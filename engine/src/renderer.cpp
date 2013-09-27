#include "renderer.h"

#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

#include "resource/texture.h"
#include "resource/cubemaptexture.h"
#include "abstractscene.h"
#include "entity/camera.h"
#include "entity/light.h"
#include "effect/hdr.h"

#include "resource/resourcedespatcher.h"

#include <cassert>

using namespace Engine;

Renderer::Renderer(ResourceDespatcher* despatcher)
    : lightningTech_(despatcher), shadowTech_(despatcher), skyboxTech_(despatcher),
    errorMaterial_(despatcher)
{
    framebuffer_ = 0;
    renderTexture_ = 0;
    depthRenderbuffer_ = 0;
    width_ = 0;
    height_ = 0;

    // HDR postfx
    postfxChain_.push_back(new Effect::Hdr(despatcher));

    // Program for debugging shadow maps
    nullTech_.addShaderFromSourceFile(QOpenGLShader::Vertex, RESOURCE_PATH("shaders/passthrough.vert"));
    nullTech_.addShaderFromSourceFile(QOpenGLShader::Fragment, RESOURCE_PATH("shaders/shadowmap.frag"));
    nullTech_.link();

    // Cache error material
    errorMaterial_.setTexture(Material::TEXTURE_DIFFUSE,
        despatcher->get<Texture>(RESOURCE_PATH("images/pink.png")));

    errorMaterial_.setTexture(Material::TEXTURE_NORMALS,
        despatcher->get<Texture>(RESOURCE_PATH("images/blue.png")));

    errorMaterial_.setTexture(Material::TEXTURE_SPECULAR,
        despatcher->get<Texture>(RESOURCE_PATH("images/white.png")));
}

Renderer::~Renderer()
{
    destroyBuffers();

    for(Effect::Postfx* fx : postfxChain_)
        delete fx;
}

void Renderer::prepareScene(AbstractScene* scene)
{
    rootNode_.removeAllChildren();

    if(scene != nullptr)
    {
        scene->prepareScene(&rootNode_);
    }
}

void Renderer::destroyBuffers()
{
    gl->glDeleteFramebuffers(1, &framebuffer_);
    gl->glDeleteTextures(1, &renderTexture_);
    gl->glDeleteRenderbuffers(1, &depthRenderbuffer_);
}

bool Renderer::initialize(int width, int height, int samples)
{
    if(width <= 0 || height <= 0 || samples < 0)
        return false;

    destroyBuffers();
    width_ = width;
    height_ = height;

    if(!shadowTech_.initSpotLights(width, height, Technique::BasicLightning::MAX_SPOT_LIGHTS))
        return false;

    // Initialize textures
    gl->glGenTextures(1, &renderTexture_);

    // Target texture
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderTexture_);
    gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, width, height, GL_TRUE);
	gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // Create depthbuffer
    gl->glGenRenderbuffers(1, &depthRenderbuffer_);
    gl->glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer_);
    gl->glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, width, height);

    // Initialize framebuffers
    gl->glGenFramebuffers(1, &framebuffer_);

    // Renderbuffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer_);
    gl->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture_, 0);

    if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    gl->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Initialize postprocess chain
    Effect::Postfx* fx = postfxChain_.front();

    if(!fx->initialize(width, height, samples))
        return false;

    fx->setInputTexture(renderTexture_);
    fx->setOutputFbo(0);    // Output to window

    return true;
}

void Renderer::render(AbstractScene* scene)
{
    const Entity::Camera* camera = scene->activeCamera();
    const QMatrix4x4 worldView = camera->perspective() * camera->lookAt();

    visibles_.clear();
    shadowCasters_.clear();

    // Cull visibles and shadow casters, TODO
    updateRenderQueue(&rootNode_, QMatrix4x4());

    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_FRONT);

    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LESS);

    // Pass 1
    // Render to depth buffer
    shadowMapPass(scene);

    gl->glCullFace(GL_BACK);

    // Pass 2
    // Render geometry to a multisampled framebuffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    renderPass(scene, worldView);

    // Render skybox if set
    skyboxPass(scene, worldView);

    gl->glDisable(GL_DEPTH_TEST);

    // Pass 3
    // Render postprocess chain
    for(Effect::Postfx* fx : postfxChain_)
    {
        fx->render(quad_);
    }

    //drawTextureDebug();
}

void Renderer::shadowMapPass(AbstractScene* scene)
{
    const auto& spotLights = scene->querySpotLights();
    if(!shadowTech_.enable())
    {
        return;
    }

    // Render depth for each spot light
    for(size_t i = 0; i < spotLights.size(); ++i)
    {
        shadowTech_.enableSpotLight(i, spotLights.at(i));
        gl->glClear(GL_DEPTH_BUFFER_BIT);

        for(auto it = shadowCasters_.begin(); it != shadowCasters_.end(); ++it)
        {
            const Entity::RenderList& node = (*it)->second;
            shadowTech_.setLightMVP(shadowTech_.spotLightVP(i) * (*it)->first);

            for(auto rit = node.begin(); rit != node.end(); ++rit)
            {
                (*rit)->render();
            }
        }
    }
}

void Renderer::renderPass(AbstractScene* scene, const QMatrix4x4& worldView)
{
    const auto& spotLights = scene->querySpotLights();

    if(!lightningTech_.enable())
    {
        return;
    }

    lightningTech_.setEyeWorldPos(scene->activeCamera()->position());
    lightningTech_.setTextureUnits(0, 1, 2);
    lightningTech_.setDirectionalLight(scene->queryDirectionalLight());
    lightningTech_.setPointLights(scene->queryPointLights());
    lightningTech_.setSpotLights(spotLights);

    // Bind spot light shadow maps
    for(unsigned int i = 0; i < spotLights.size(); ++i)
    {
        shadowTech_.bindSpotLight(i, GL_TEXTURE0 + Material::TEXTURE_COUNT + i);
        lightningTech_.setSpotLightShadowUnit(i, Material::TEXTURE_COUNT + i);
    }

    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glClearColor(0, 0, 0, 0);

    for(auto it = visibles_.begin(); it != visibles_.end(); ++it)
    {
        Entity::RenderList& node = it->second;

        lightningTech_.setWorldView(it->first);
        lightningTech_.setMVP(worldView * it->first);

        // Set translation matrix for each spot light
        for(unsigned int i = 0; i < spotLights.size(); ++i)
        {
            lightningTech_.setSpotLightMVP(i, shadowTech_.spotLightVP(i) * it->first);
        }

        for(auto rit = node.begin(); rit != node.end(); ++rit)
        {
            const Material::Ptr& material = (*rit)->material();

            if(!material->bind())
            {
                if(!errorMaterial_.bind())
                    continue;
            }

            lightningTech_.setMaterialAttributes(material->getAttributes());
            lightningTech_.setHasTangents((*rit)->hasTangents());

            (*rit)->render();
        }
    }
}

void Renderer::skyboxPass(AbstractScene* scene, const QMatrix4x4& worldView)
{
    if(scene->skyboxTexture() == nullptr || scene->skyboxMesh() == nullptr)
        return;

    if(!skyboxTech_.enable())
    {
        return;
    }

    QMatrix4x4 trans;
    trans.translate(scene->activeCamera()->position());

    skyboxTech_.setMVP(worldView * trans);
    skyboxTech_.setTextureUnit(0);

    if(scene->skyboxTexture()->bind(GL_TEXTURE0))
    {
        gl->glCullFace(GL_FRONT);
        gl->glDepthFunc(GL_LEQUAL);

        scene->skyboxMesh()->render();

        gl->glCullFace(GL_BACK);
    }
}

void Renderer::updateRenderQueue(Graph::SceneNode* node, const QMatrix4x4& worldView)
{
    if(node == nullptr)
    {
        return;
    }

    QMatrix4x4 nodeView = worldView * node->transformation();

    if(node->numEntities() > 0)
    {
        visibles_.push_back(std::make_pair(nodeView, Entity::RenderList()));

        for(size_t i = 0; i < node->numEntities(); ++i)
        {
            Entity::Entity* entity = node->getEntity(i);
            if(entity != nullptr)
            {
                entity->updateRenderList(visibles_.back().second);
            }
        }

        if(node->isShadowCaster())
        {
            const VisibleNode& added = visibles_.back();
            shadowCasters_.push_back(&added);
        }
    }

    // Recursively walk through child nodes
    for(size_t i = 0; i < node->numChildren(); ++i)
    {
        Graph::SceneNode* inode = dynamic_cast<Graph::SceneNode*>(node->getChild(i));
        updateRenderQueue(inode, nodeView);
    }
}

void Renderer::drawTextureDebug()
{
    shadowTech_.bindSpotLight(0, GL_TEXTURE0);
    nullTech_.bind();
    nullTech_.setUniformValue("gShadowMap", 0);

    gl->glViewport(0, 0, width_ / 3, height_ / 3);

    quad_.render();

    gl->glViewport(0, 0, width_, height_);
}