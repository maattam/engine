#include "quadlighting.h"

#include "resourcedespatcher.h"
#include "entity/camera.h"
#include "entity/light.h"
#include "gbuffer.h"
#include "scene/visiblescene.h"
#include "renderable/primitive.h"

using namespace Engine;

QuadLighting::QuadLighting(Renderer* renderer, GBuffer& gbuffer, ResourceDespatcher& despatcher)
    : LightingStage(renderer), gbuffer_(gbuffer), fbo_(0), quad_(Renderable::Primitive<Renderable::Quad>::instance())
{
    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/dsmaterial.vert"), Shader::Type::Vertex));
    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/dsillumination.frag"), Shader::Type::Fragment));

    lightningTech_.setGBuffer(&gbuffer);
}

QuadLighting::~QuadLighting()
{
}

bool QuadLighting::setViewport(const QRect& viewport, unsigned int samples)
{
    lightningTech_.setSampleCount(samples);

    return LightingStage::setViewport(viewport, samples);
}

void QuadLighting::setRenderTarget(GLuint fbo)
{
    LightingStage::setRenderTarget(fbo);
    fbo_ = fbo;
}

void QuadLighting::render(Entity::Camera* camera)
{
    LightingStage::render(camera);

    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    gl->glClear(GL_COLOR_BUFFER_BIT);

    if(!lightningTech_.enable())
    {
        return;
    }

    lightningTech_.setProjMatrix(camera->projection());
    lightningTech_.setViewMatrix(camera->view());
    gbuffer_.bindTextures();

    // Render directional light
    if(scene_->directionalLight() != nullptr)
    {
        lightningTech_.enableDirectionalLight(*scene_->directionalLight());
        quad_->render();
    }

    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_ONE, GL_ONE);

    // Blend point lights
    for(Entity::Light* light : pointLights_)
    {
        lightningTech_.enablePointLight(*light);
        quad_->render();
    }

    // Blend spotlights
    for(Entity::Light* light : spotLights_)
    {
        lightningTech_.enableSpotLight(*light);
        quad_->render();
    }

    gl->glDisable(GL_BLEND);

    spotLights_.clear();
    pointLights_.clear();
}

void QuadLighting::visit(Entity::Light& light)
{
    if(light.type() == Entity::Light::LIGHT_POINT)
    {
        pointLights_.push_back(&light);
    }

    else if(light.type() == Entity::Light::LIGHT_SPOT)
    {
        spotLights_.push_back(&light);
    }
}