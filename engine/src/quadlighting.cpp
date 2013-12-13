#include "quadlighting.h"

#include "resourcedespatcher.h"
#include "entity/camera.h"
#include "entity/light.h"
#include "gbuffer.h"

#include <QOpenGLFramebufferObject>

using namespace Engine;

QuadLighting::QuadLighting(Renderer* renderer, GBuffer& gbuffer, ResourceDespatcher& despatcher)
    : LightingStage(renderer), gbuffer_(gbuffer), fbo_(nullptr)
{
    materialShader_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/dsmaterial.vert"), Shader::Type::Vertex));
    materialShader_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/dsmaterial.frag"), Shader::Type::Fragment));

    materialShader_.setGBuffer(&gbuffer);
    materialShader_.setDepthRange(0, 1);
}

QuadLighting::~QuadLighting()
{
}

bool QuadLighting::setViewport(const QRect& viewport, unsigned int samples)
{
    materialShader_.setSampleCount(samples);

    return LightingStage::setViewport(viewport, samples);
}

void QuadLighting::setOutputFBO(QOpenGLFramebufferObject* fbo)
{
    LightingStage::setOutputFBO(fbo);

    fbo_ = fbo;
}

void QuadLighting::render(Entity::Camera* camera)
{
    lights_.clear();
    LightingStage::render(camera);

    // For each visited light, render..

    if(fbo_ == nullptr || !fbo_->bind())
    {
        gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    gl->glClear(GL_COLOR_BUFFER_BIT);

    // Output each GBuffer component to screen for now
    if(!materialShader_.enable())
    {
        return;
    }

    materialShader_.setProjMatrix(camera->projection());
    materialShader_.setLightDirection(camera->view().mapVector(QVector3D(0.0f, -1.0f, -0.09f)));

    gbuffer_.bindTextures();
    quad_.render();
}

void QuadLighting::visit(Entity::Light& light)
{
    lights_.push_back(&light);
}