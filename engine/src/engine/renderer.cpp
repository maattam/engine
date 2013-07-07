#include "renderer.h"

#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

#include "texture.h"
#include "renderable.h"
#include "abstractscene.h"
#include "camera.h"
#include "light.h"
#include "effect/hdr.h"

using namespace Engine;

Renderer::Renderer(QOpenGLFunctions_4_2_Core* funcs)
    : gl(funcs), quad_(funcs), shadowTech_(funcs)
{
    framebuffer_ = 0;
    renderTexture_ = 0;
    depthRenderbuffer_ = 0;
    width_ = 0;
    height_ = 0;

    // HDR postfx
    postfxChain_.push_back(new Hdr(funcs));

    nullTech_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/passthrough.vert");
    nullTech_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shadowmap.frag");
    nullTech_.link();
}

Renderer::~Renderer()
{
    destroyBuffers();

    for(Postfx* fx : postfxChain_)
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

    if(!lightningTech_.init())
        return false;

    if(!shadowTech_.init(width, height))
        return false;

    destroyBuffers();
    width_ = width;
    height_ = height;

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

    // Render buffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer_);
    gl->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture_, 0);

    if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    gl->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Initialize postprocess chain
    Postfx* fx = postfxChain_.front();

    if(!fx->initialize(width, height, samples))
        return false;

    fx->setInputTexture(renderTexture_);
    fx->setOutputFbo(0);    // Output to window

    return true;
}

void Renderer::render(AbstractScene* scene)
{
    renderQueue_.clear();
    updateRenderQueue(&rootNode_, rootNode_.transformation());

    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_FRONT);

    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LEQUAL);

    // Pass 1
    // Render to depth buffer
    shadowMapPass(scene);

    gl->glCullFace(GL_BACK);

    // Pass 2
    // Render the geometry to a multisampled framebuffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    renderPass(scene);

    gl->glDisable(GL_DEPTH_TEST);

    // Pass 3
    // Render postprocess chain
    for(Postfx* fx : postfxChain_)
    {
        fx->render(quad_);
    }

    //drawTextureDebug();
}

void Renderer::shadowMapPass(AbstractScene* scene)
{
    const SpotLight& light = scene->querySpotLights().front();

    QMatrix4x4 look;
    lightVP_.setToIdentity();
    lightVP_.perspective(45.0f, static_cast<float>(width_) / height_, 1.0f, 50.0f);
    look.lookAt(light.position, light.position + light.direction, QVector3D(0, 1, 0));
    lightVP_ *= look;

    shadowTech_.enable();

    gl->glClear(GL_DEPTH_BUFFER_BIT);

    for(auto it = renderQueue_.begin(); it != renderQueue_.end(); ++it)
    {
        RenderList& node = it->second;

        for(auto rit = node.begin(); rit != node.end(); ++rit)
        {
            shadowTech_.setMVP(lightVP_ * it->first);
            (*rit)->render();
        }
    }
}

void Renderer::renderPass(AbstractScene* scene)
{
    QMatrix4x4 v = scene->activeCamera()->lookAt();
    QMatrix4x4 vp = scene->activeCamera()->perspective() * v;

    lightningTech_.enable();

    lightningTech_.setEyeWorldPos(scene->activeCamera()->position());
    lightningTech_.setTextureUnits(0, 1, 2);
    lightningTech_.setShadowMapTextureUnit(3);
    lightningTech_.setDirectionalLight(scene->queryDirectionalLight());
    lightningTech_.setPointLights(scene->queryPointLights());
    lightningTech_.setSpotLights(scene->querySpotLights());

    shadowTech_.bindTexture(GL_TEXTURE3);

    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glClearColor(0, 0, 0, 0);

    for(auto it = renderQueue_.begin(); it != renderQueue_.end(); ++it)
    {
        RenderList& node = it->second;
        lightningTech_.setWorldView(it->first);

        for(auto rit = node.begin(); rit != node.end(); ++rit)
        {
            lightningTech_.setMVP(vp * it->first);
            lightningTech_.setLightMVP(lightVP_ * it->first);

            const Material::Ptr& material = (*rit)->material();

            lightningTech_.setMaterialAttributes(material->getAttributes());
            material->getTexture(Material::TEXTURE_DIFFUSE)->bind(GL_TEXTURE0);
            material->getTexture(Material::TEXTURE_SPECULAR)->bind(GL_TEXTURE2);

            if((*rit)->hasTangents())
            {
                material->getTexture(Material::TEXTURE_NORMALS)->bind(GL_TEXTURE1);
                lightningTech_.setHasTangents(true);
            }

            else
            {
                lightningTech_.setHasTangents(false);
            }

            (*rit)->render();
        }
    }
}

void Renderer::updateRenderQueue(SceneNode* node, const QMatrix4x4& worldView)
{
    if(node == nullptr)
    {
        return;
    }

    if(node->numEntities() > 0)
    {
        renderQueue_.push_back(std::make_pair(worldView * node->transformation(), RenderList()));

        for(int i = 0; i < node->numEntities(); ++i)
        {
            Entity* entity = node->getEntity(i);
            if(entity != nullptr)
            {
                entity->updateRenderList(renderQueue_.back().second);
            }
        }
    }

    // Follow child nodes
    for(int i = 0; i < node->numChildren(); ++i)
    {
        SceneNode* inode = dynamic_cast<SceneNode*>(node->getChild(i));
        updateRenderQueue(inode, worldView * node->transformation());
    }
}

void Renderer::drawTextureDebug()
{
    shadowTech_.bindTexture(GL_TEXTURE0);
    nullTech_.bind();
    nullTech_.setUniformValue("gShadowMap", 0);

    gl->glViewport(0, 0, width_ / 3, height_ / 3);

    quad_.render();

    gl->glViewport(0, 0, width_, height_);
}