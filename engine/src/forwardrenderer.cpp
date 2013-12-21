#include "forwardrenderer.h"

#include <QMatrix4x4>
#include <QDebug>

#include "entity/camera.h"
#include "entity/light.h"
#include "graph/scenenode.h"
#include "renderable/renderable.h"
#include "scene/visiblescene.h"
#include "resourcedespatcher.h"
#include "texture2dresource.h"

using namespace Engine;

ForwardRenderer::ForwardRenderer(ResourceDespatcher& despatcher)
    : scene_(nullptr), samples_(1), fbo_(0)
{
    // Cache error material
    errorMaterial_.setTexture(Material::TEXTURE_DIFFUSE,
        despatcher.get<Texture2DResource>(RESOURCE_PATH("images/pink.png"), TC_SRGBA));

    // ShadowMap shaders
    shadowTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/shadowmap.vert"), Shader::Type::Vertex));
    shadowTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/shadowmap.frag"), Shader::Type::Fragment));

    // BasicLightning shaders
    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/basiclightning.vert"), Shader::Type::Vertex));
    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/basiclightning.frag"), Shader::Type::Fragment));
}

ForwardRenderer::~ForwardRenderer()
{
}

bool ForwardRenderer::setViewport(const QRect& viewport, unsigned int samples)
{
    viewport_ = viewport;
    samples_ = samples;

    if(!initialiseBuffers(viewport.width(), viewport.height(), samples))
    {
        return false;
    }

    return true;
}

void ForwardRenderer::setScene(VisibleScene* scene)
{
    scene_ = scene;
}

void ForwardRenderer::setRenderTarget(GLuint fbo)
{
    fbo_ = fbo;
}

void ForwardRenderer::visit(Entity::Light& light)
{
    lights_.push_back(&light);
}

bool ForwardRenderer::initialiseBuffers(unsigned int width, unsigned int height, unsigned int samples)
{
    if(!shadowTech_.initSpotLights(1024, 1024, Technique::BasicLightning::MAX_SPOT_LIGHTS))
        return false;

    if(!shadowTech_.initDirectionalLight(4096, 4096))
        return false;

    return true;
}

void ForwardRenderer::render(Entity::Camera* camera)
{
    // Gather visible lights
    scene_->addVisitor(this);

    // Cull visibles
    RenderQueue renderQueue;
    scene_->queryVisibles(camera->worldView(), renderQueue);
    scene_->removeVisitor(this);

    gl->glEnable(GL_CULL_FACE);
    gl->glEnable(GL_DEPTH_TEST);

    // Pass 1
    // Render to depth buffer
    shadowMapPass();

    // Pass 2
    // Render geometry and lightning
    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    renderPass(camera, renderQueue);

    gl->glDisable(GL_DEPTH_TEST);
    gl->glDisable(GL_CULL_FACE);

    lights_.clear();
}

void ForwardRenderer::shadowMapPass()
{
    if(!shadowTech_.enable())
    {
        return;
    }

    gl->glCullFace(GL_FRONT);

    // Render depth for each spot light
    int spotLightIndex = 0;
    for(Entity::Light* light : lights_)
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
    // Prepare OpenGL state for render pass
    gl->glViewport(viewport_.x(), viewport_.y(), viewport_.width(), viewport_.height());
    gl->glClearColor(0.0063f, 0.0063f, 0.0063f, 0);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!lightningTech_.enable())
    {
        return;
    }

    lightningTech_.setEyeWorldPos(camera->position());
    lightningTech_.setTextureUnits(0, 1, 2, 3);
    lightningTech_.setDirectionalLight(scene_->directionalLight());
    lightningTech_.setPointAndSpotLights(lights_);
    lightningTech_.setShadowEnabled(true);

    // Bind directional light shadow map
    shadowTech_.bindDirectionalLight(GL_TEXTURE0 + Material::TEXTURE_COUNT);
    lightningTech_.setDirectionalLightShadowUnit(Material::TEXTURE_COUNT);

    // Bind spot light shadow maps after material samplers
    for(size_t i = 0; i < shadowTech_.numSpotLights(); ++i)
    {
        GLenum index = Material::TEXTURE_COUNT + i + 1;

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

    lightningTech_.setHasTangents(renderable->hasTangents() && material->hasTexture(Material::TEXTURE_NORMALS));
    lightningTech_.setMaterialAttributes(material->attributes());

    renderable->render();
}