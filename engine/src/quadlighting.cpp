#include "quadlighting.h"

#include "resourcedespatcher.h"
#include "graph/camera.h"
#include "graph/light.h"
#include "gbuffer.h"
#include "renderable/primitive.h"

using namespace Engine;

QuadLighting::QuadLighting(Renderer* renderer, GBuffer& gbuffer, ResourceDespatcher& despatcher)
    : RenderStage(renderer), gbuffer_(gbuffer), fbo_(0), directionalLight_(nullptr),
    quad_(Renderable::Primitive<Renderable::Quad>::instance())
{
    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/dsillumination.vert"), Shader::Type::Vertex));
    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/dsillumination.frag"), Shader::Type::Fragment));

    lightningTech_.setGBuffer(&gbuffer);
}

QuadLighting::~QuadLighting()
{
}

bool QuadLighting::setViewport(const QRect& viewport, unsigned int samples)
{
    lightningTech_.setSampleCount(samples);
    viewport_ = viewport;

    return RenderStage::setViewport(viewport, samples);
}

void QuadLighting::setRenderTarget(GLuint fbo)
{
    RenderStage::setRenderTarget(fbo);
    fbo_ = fbo;
}

void QuadLighting::render(Graph::Camera* camera)
{
    RenderStage::render(camera);

    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    gl->glClear(GL_COLOR_BUFFER_BIT);

    if(!lightningTech_.enable())
    {
        return;
    }

    lightningTech_.setProjMatrix(camera->projection());
    lightningTech_.setViewMatrix(camera->view());
    lightningTech_.setViewport(viewport_);

    gbuffer_.bindTextures();

    // Render directional light
    if(directionalLight_ != nullptr)
    {
        lightningTech_.enableDirectionalLight(*directionalLight_);
        quad_->render();
    }

    lightningTech_.setCameraAxes(camera->up(), camera->right());
    lightningTech_.setViewProjMatrix(camera->worldView());

    // Cache view matrix for positioning quads
    viewMatrix_ = camera->view();
    viewMatrixInverse_ = viewMatrix_.inverted();

    gl->glEnable(GL_BLEND);
    gl->glBlendEquation(GL_FUNC_ADD);
    gl->glBlendFunc(GL_ONE, GL_ONE);

    // Blend spotlights
    for(Graph::Light* light : spotLights_)
    {
        setSpotLightExtents(camera, light);
        lightningTech_.enableSpotLight(*light);

        quad_->render();
    }

    // Blend point lights
    for(Graph::Light* light : pointLights_)
    {
        setPointLightExtents(camera, light);
        lightningTech_.enablePointLight(*light);

        quad_->render();
    }

    gl->glDisable(GL_BLEND);

    spotLights_.clear();
    pointLights_.clear();
    directionalLight_ = nullptr;
}

void QuadLighting::setLights(const QVector<LightData>& lights)
{
    for(const LightData& lightData : lights)
    {
        if(lightData.light->type() == Graph::Light::LIGHT_DIRECTIONAL)
        {
            directionalLight_ = lightData.light;
        }

        else if(lightData.light->type() == Graph::Light::LIGHT_POINT)
        {
            pointLights_.push_back(lightData.light);
        }

        else if(lightData.light->type() == Graph::Light::LIGHT_SPOT)
        {
            spotLights_.push_back(lightData.light);
        }
    }
}

void QuadLighting::setPointLightExtents(Graph::Camera* camera, Graph::Light* light)
{
    lightningTech_.setQuadExtents(light->cutoffDistance(),
        calcQuadPosition(camera, light->position(), light->cutoffDistance()));
}

void QuadLighting::setSpotLightExtents(Graph::Camera* camera, Graph::Light* light)
{
    QVector3D spotCenter = light->position() + light->direction() * light->cutoffDistance() / 2.0f;

    lightningTech_.setQuadExtents(light->cutoffDistance(),
        calcQuadPosition(camera, spotCenter, light->cutoffDistance()));
}

QVector3D QuadLighting::calcQuadPosition(Graph::Camera* camera, const QVector3D& lightPos, float lightCutoff)
{
    const float clipMargin = camera->nearPlane() + 0.0001f;

    QVector3D eyePos = camera->position() + camera->direction() * clipMargin;
    QVector3D extent = eyePos - lightPos;

    QVector3D quadPos;

    // Clamp quad Z to near clip plane Z if we are inside the light's extents
    if(extent.length() <= lightCutoff)
    {
        QVector3D lightPosView = viewMatrix_ * lightPos;

        // Move the quad slightly in front of the clip plane because objects on the clip plane are culled.
        lightPosView.setZ(-clipMargin);

        quadPos = viewMatrixInverse_ * lightPosView;
    }

    // Set quad Z to min Z of light
    else
    {
        quadPos = lightPos + extent.normalized() * lightCutoff;
    }

    return quadPos;
}