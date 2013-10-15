#include "renderer.h"

#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

#include "texture2D.h"
#include "cubemaptexture.h"
#include "abstractscene.h"
#include "entity/camera.h"
#include "entity/light.h"
#include "effect/hdr.h"

#include "resourcedespatcher.h"

#include <cassert>

using namespace Engine;

Renderer::Renderer(ResourceDespatcher* despatcher)
    : lightningTech_(despatcher), shadowTech_(despatcher), skyboxTech_(despatcher),
    errorMaterial_(despatcher), aabbMaterial_(despatcher), flags_(0)
{
    framebuffer_ = 0;
    renderTexture_ = 0;
    depthRenderbuffer_ = 0;
    width_ = 0;
    height_ = 0;

    // HDR postfx
    postfxChain_.push_back(new Effect::Hdr(despatcher, 4));

    // Program for debugging shadow maps
    nullTech_.addShaderFromSourceFile(QOpenGLShader::Vertex, RESOURCE_PATH("shaders/passthrough.vert"));
    nullTech_.addShaderFromSourceFile(QOpenGLShader::Fragment, RESOURCE_PATH("shaders/shadowmap.frag"));
    nullTech_.link();

    // AABB debugging tech
    aabbTech_.addShaderFromSourceFile(QOpenGLShader::Vertex, RESOURCE_PATH("shaders/aabb.vert"));
    aabbTech_.addShaderFromSourceFile(QOpenGLShader::Fragment, RESOURCE_PATH("shaders/aabb.frag"));
    aabbTech_.link();

    // Cache error material
    errorMaterial_.setTexture(Material::TEXTURE_DIFFUSE,
        despatcher->get<Texture2D>(RESOURCE_PATH("images/pink.png")));

    // AABB box
    aabbMaterial_.setAmbientColor(QVector3D(0, 0, 1));
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

    if(!shadowTech_.initSpotLights(1024, 1024, Technique::BasicLightning::MAX_SPOT_LIGHTS))
        return false;

    if(!shadowTech_.initDirectionalLight(2048, 2048))
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
    //gl->glEnable(GL_FRAMEBUFFER_SRGB);

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
    Entity::Camera* camera = scene->activeCamera();
    const QMatrix4x4 worldView = camera->perspective() * camera->lookAt();

    visibles_.clear();
    shadowCasters_.clear();
    aabbDebug_.clear();

    // Cull visibles and shadow casters, TODO
    updateRenderQueue(&rootNode_, QMatrix4x4());

    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LESS);

    // Pass 1
    // Render to depth buffer
    shadowMapPass(scene);

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

    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_FRONT);

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
                (*rit).second->render();
            }
        }
    }

    // Render depth for directional light
    shadowTech_.enableDirectinalLight(scene->queryDirectionalLight());
    gl->glClear(GL_DEPTH_BUFFER_BIT);

    for(auto it = shadowCasters_.begin(); it != shadowCasters_.end(); ++it)
    {
        const Entity::RenderList& node = (*it)->second;
        shadowTech_.setLightMVP(shadowTech_.directionalLightVP() * (*it)->first);

        for(auto rit = node.begin(); rit != node.end(); ++rit)
        {
            (*rit).second->render();
        }
    }
}

void Renderer::renderPass(AbstractScene* scene, const QMatrix4x4& worldView)
{
    const auto& spotLights = scene->querySpotLights();

    // Prepare OpenGL state for render pass
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glClearColor(0.0063f, 0.0063f, 0.0063f, 0);

    gl->glCullFace(GL_BACK);
    gl->glViewport(0, 0, width_, height_);

    if(!lightningTech_.enable())
    {
        return;
    }

    lightningTech_.setEyeWorldPos(scene->activeCamera()->position());
    lightningTech_.setTextureUnits(0, 1, 2);
    lightningTech_.setDirectionalLight(scene->queryDirectionalLight());
    lightningTech_.setPointLights(scene->queryPointLights());
    lightningTech_.setSpotLights(spotLights);

    // Bind directional light shadow map
    shadowTech_.bindDirectionalLight(GL_TEXTURE0 + Material::TEXTURE_COUNT);
    lightningTech_.setDirectionalLightShadowUnit(Material::TEXTURE_COUNT);

    // Bind spot light shadow maps after material samplers
    for(unsigned int i = 0; i < spotLights.size(); ++i)
    {
        shadowTech_.bindSpotLight(i, GL_TEXTURE0 + Material::TEXTURE_COUNT + i + 1);
        lightningTech_.setSpotLightShadowUnit(i, Material::TEXTURE_COUNT + i + 1);
    }

    // Set polygonmode if wireframe mode is enabled
    if(flags_ & DEBUG_WIREFRAME)
    {
        gl->glDisable(GL_CULL_FACE);
        gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        Material::Attributes attrib;
        attrib.ambientColor = QVector3D(0.3f, 0.3f, 0.3f);  // Hilight polygons slightly

        lightningTech_.setMaterialAttributes(attrib);
    }

    // Render visibles
    for(auto it = visibles_.begin(); it != visibles_.end(); ++it)
    {
        lightningTech_.setWorldView(it->first);
        lightningTech_.setMVP(worldView * it->first);

        // Set translation matrix for each spot light
        for(unsigned int i = 0; i < spotLights.size(); ++i)
        {
            lightningTech_.setSpotLightMVP(i, shadowTech_.spotLightVP(i) * it->first);
        }

        // Set directional light mvp
        lightningTech_.setDirectionalLightMVP(shadowTech_.directionalLightVP() * it->first);

        renderNode(it->second);
    }

    // Reset polygonmode
    if(flags_ & DEBUG_WIREFRAME)
    {
        gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        gl->glEnable(GL_CULL_FACE);
    }

    // Draw bounding boxes
    if(flags_ & DEBUG_AABB)
    {
        aabbTech_.bind();
        aabbTech_.setUniformValue("gColor", aabbMaterial_.attributes().ambientColor);

        gl->glDisable(GL_CULL_FACE);
        gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for(auto it = aabbDebug_.begin(); it != aabbDebug_.end(); ++it)
        {
            aabbTech_.setUniformValue("gMVP", worldView * (*it));
            aabbBox_.render();
        }

        gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        gl->glEnable(GL_CULL_FACE);
    }
}

void Renderer::renderNode(const Entity::RenderList& node)
{
    for(auto it = node.begin(); it != node.end(); ++it)
    {
        Material* material = (*it).first;
        Renderable::Renderable* renderable = (*it).second;

        if(!material->bind())
        {
            material = &errorMaterial_;     // Draw error material to indicate a problem with the object

            if(!errorMaterial_.bind())      // We can't fail further
                continue;
        }

        lightningTech_.setHasTangents(renderable->hasTangents() && material->hasNormals());

        // Ignore material attributes in wireframe mode
        if(!(flags_ & DEBUG_WIREFRAME))
        {
            lightningTech_.setMaterialAttributes(material->attributes());
        }

        renderable->render();
    }
}

void Renderer::skyboxPass(AbstractScene* scene, const QMatrix4x4& worldView)
{
    if(flags_ & DEBUG_WIREFRAME)    // Don't draw skybox in wireframe mode to help debugging
        return;

    if(scene->skyboxTexture() == nullptr || scene->skyboxMesh() == nullptr)
        return;

    if(!skyboxTech_.enable())
        return;

    QMatrix4x4 trans;
    trans.translate(scene->activeCamera()->position());

    skyboxTech_.setMVP(worldView * trans);
    skyboxTech_.setTextureUnit(0);

    if(scene->skyboxTexture()->bindActive(GL_TEXTURE0))
    {
        // We want to see the skybox texture from the inside
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

    // Push entities from the node into the render queue
    if(node->numEntities() > 0)
    {
        visibles_.push_back(std::make_pair(nodeView, Entity::RenderList()));

        for(size_t i = 0; i < node->numEntities(); ++i)
        {
            Entity::Entity* entity = node->getEntity(i);
            if(entity != nullptr)
            {
                entity->updateRenderList(visibles_.back().second);

                if(flags_ & DEBUG_AABB)
                {
                    // Stretch box to fit AABB
                    addAABBDebug(nodeView, entity->boundingBox());
                }
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
    shadowTech_.bindDirectionalLight(GL_TEXTURE0);
    nullTech_.bind();
    nullTech_.setUniformValue("gShadowMap", 0);

    gl->glViewport(0, 0, 512, 512);

    quad_.render();

    gl->glViewport(0, 0, width_, height_);
}

void Renderer::setFlags(unsigned int flags)
{
    flags_ = flags;
}

unsigned int Renderer::flags() const
{
    return flags_;
}

void Renderer::addAABBDebug(const QMatrix4x4& trans, const Entity::AABB& aabb)
{
    if(aabb.width() <= 0)
        return;

    QMatrix4x4 scale;
    scale.translate(aabb.center());
    scale.scale(0.5f);  // Our bounding box model is 2 units wide
    scale.scale(aabb.width(), aabb.height(), aabb.depth());   // Display over mesh surface

    aabbDebug_.push_back(trans * scale);
}