#include "forwardrenderer.h"

#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QDebug>

#include "texture2D.h"
#include "cubemaptexture.h"
#include "effect/postfx.h"
#include "entity/camera.h"
#include "entity/light.h"
#include "effect/hdr.h"
#include "graph/scenenode.h"

#include "resourcedespatcher.h"

#include <cassert>

using namespace Engine;

ForwardRenderer::ForwardRenderer(ResourceDespatcher* despatcher)
    : scene_(nullptr), flags_(0), errorMaterial_(despatcher),
    lightningTech_(despatcher), shadowTech_(despatcher), skyboxTech_(despatcher)
{
    framebuffer_ = 0;
    renderTexture_ = 0;
    depthRenderbuffer_ = 0;

    // HDR postfx
    postfxChain_.push_back(new Effect::Hdr(despatcher, 4));

    // Program for debugging shadow maps
    nullTech_.addShaderFromSourceFile(QOpenGLShader::Vertex, RESOURCE_PATH("shaders/passthrough.vert"));
    nullTech_.addShaderFromSourceFile(QOpenGLShader::Fragment, RESOURCE_PATH("shaders/shadowmap.frag"));
    nullTech_.link();

    // Cache error material
    errorMaterial_.setTexture(Material::TEXTURE_DIFFUSE,
        despatcher->get<Texture2D>(RESOURCE_PATH("images/pink.png"), true));

    flags_ |= RENDER_SHADOWS;
}

ForwardRenderer::~ForwardRenderer()
{
    destroyBuffers();

    for(Effect::Postfx* fx : postfxChain_)
        delete fx;
}

bool ForwardRenderer::setViewport(unsigned int width, unsigned int height, unsigned int samples,
        unsigned int left = 0, unsigned int top = 0)
{
    viewport_ = QRect(left, top, width, height);
    return initialiseBuffers(width, height, samples);
}

void ForwardRenderer::setScene(VisibleScene* scene)
{
    scene_ = scene;
}

void ForwardRenderer::destroyBuffers()
{
    gl->glDeleteFramebuffers(1, &framebuffer_);
    gl->glDeleteTextures(1, &renderTexture_);
    gl->glDeleteRenderbuffers(1, &depthRenderbuffer_);
}

bool ForwardRenderer::initialiseBuffers(unsigned int width, unsigned int height, unsigned int samples)
{
    if(width <= 0 || height <= 0 || samples < 0)
        return false;

    if(!shadowTech_.initSpotLights(1024, 1024, Technique::BasicLightning::MAX_SPOT_LIGHTS))
        return false;

    if(!shadowTech_.initDirectionalLight(4096, 4096))
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
    gl->glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);

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

void ForwardRenderer::render(Entity::Camera* camera)
{
    // Cull visibles
    RenderQueue renderQueue;
    scene_->queryVisibles(camera->worldView(), renderQueue);

    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LESS);

    // Pass 1
    // Render to depth buffer
    shadowMapPass();

    // Pass 2
    // Render geometry to a multisampled framebuffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    renderPass(camera, renderQueue);

    // Render skybox if set
    skyboxPass(camera);

    gl->glDisable(GL_DEPTH_TEST);

    // Pass 3
    // Render postprocess chain
    for(Effect::Postfx* fx : postfxChain_)
    {
        fx->render(quad_);
    }
}

void ForwardRenderer::shadowMapPass()
{
    const auto& lights = scene_->queryLights();

    if(!shadowTech_.enable() || !(flags_ & RENDER_SHADOWS))
    {
        return;
    }

    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_FRONT);

    // Render depth for each spot light
    int spotLightIndex = 0;
    for(const Entity::Light* light : lights)
    {
        // Ignore other lights for now
        if(light->type() != Entity::Light::LIGHT_SPOT)
        {
            continue;
        }

        shadowTech_.renderSpotLight(spotLightIndex++, light, scene_);
    }

    // Render depth for directional light
    auto directionalLight = scene_->directionalLight();
    if(directionalLight == nullptr)
    {
        return;
    }

    shadowTech_.renderDirectinalLight(directionalLight, scene_);

    gl->glCullFace(GL_BACK);
}

void ForwardRenderer::renderPass(Entity::Camera* camera, const RenderQueue& queue)
{
    const auto& lights = scene_->queryLights();

    // Prepare OpenGL state for render pass
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glClearColor(0.0063f, 0.0063f, 0.0063f, 0);

    gl->glViewport(viewport_.x(), viewport_.y(), viewport_.width(), viewport_.height());

    if(!lightningTech_.enable())
    {
        return;
    }

    lightningTech_.setEyeWorldPos(camera->position());
    lightningTech_.setTextureUnits(0, 1, 2, 3);
    lightningTech_.setDirectionalLight(scene_->directionalLight());
    lightningTech_.setPointAndSpotLights(scene_->queryLights());
    lightningTech_.setShadowEnabled(flags_ & RENDER_SHADOWS);

    // Bind directional light shadow map
    shadowTech_.bindDirectionalLight(GL_TEXTURE0 + Material::TEXTURE_COUNT);
    lightningTech_.setDirectionalLightShadowUnit(Material::TEXTURE_COUNT);

    // Bind spot light shadow maps after material samplers
    for(size_t i = 0; i < shadowTech_.numSpotLights(); ++i)
    {
        int index = Material::TEXTURE_COUNT + i + 1;

        shadowTech_.bindSpotLight(i, GL_TEXTURE0 + index);
        lightningTech_.setSpotLightShadowUnit(i, index);
    }

    // Render visibles
    for(auto it = queue.begin(); it != queue.end(); ++it)
    {
        lightningTech_.setWorldView(*it->modelView);
        lightningTech_.setMVP(camera->worldView() * *it->modelView);

        // Set transformation matrix for each spot light
        for(size_t i = 0; i < shadowTech_.numSpotLights(); ++i)
        {
            const QMatrix4x4& spotVP = shadowTech_.spotLightVP(i);
            lightningTech_.setSpotLightMVP(i, spotVP * *it->modelView);
        }

        // Set directional light mvp
        lightningTech_.setDirectionalLightMVP(shadowTech_.directionalLightVP() * *it->modelView);

        renderNode(*it);
    }
}

void ForwardRenderer::renderNode(const RenderQueue::RenderItem& node)
{
    Material* material = node.material;
    Renderable::Renderable* renderable = node.renderable;

    if(!material->bind())
    {
        material = &errorMaterial_;     // Draw error material to indicate a problem with the object

        if(!errorMaterial_.bind())      // We can't fail further
            return;
    }

    lightningTech_.setHasTangents(renderable->hasTangents() && material->hasNormals());
    lightningTech_.setMaterialAttributes(material->attributes());

    renderable->render();
}

void ForwardRenderer::skyboxPass(Entity::Camera* camera)
{
    if(!skyboxTech_.enable())
        return;

    skyboxTech_.render(camera, scene_->skybox());
}

void ForwardRenderer::setFlags(unsigned int flags)
{
    flags_ = flags;
}

unsigned int ForwardRenderer::flags() const
{
    return flags_;
}