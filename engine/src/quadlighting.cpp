#include "quadlighting.h"

#include "resourcedespatcher.h"
#include "graph/camera.h"
#include "graph/light.h"
#include "gbuffer.h"
#include "renderable/primitive.h"
#include "shadowstage.h"

#include "scene/sceneobservable.h"

using namespace Engine;

QuadLighting::QuadLighting(Renderer* renderer, GBuffer& gbuffer, ResourceDespatcher& despatcher, unsigned int samples)
    : RenderStage(renderer), gbuffer_(gbuffer), fbo_(0), directionalLight_(nullptr), camera_(nullptr), observable_(nullptr),
    shadowStage_(nullptr), quad_(Renderable::Primitive<Renderable::Quad>::instance()), lightningTech_()
{
    lightningTech_.setGBuffer(&gbuffer);

    ShaderData::DefineMap shaderDefines;
    shaderDefines.insert("SAMPLES", samples);

    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/dsillumination.vert"), Shader::Type::Vertex));
    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/dsillumination.frag"), shaderDefines, Shader::Type::Fragment));
}

QuadLighting::~QuadLighting()
{
    if(observable_ != nullptr)
    {
        // TODO: Detach visitors automatically
        observable_->removeVisitor(this);
    }
}

void QuadLighting::setObservable(SceneObservable* observable)
{
    observable->addVisitor(this);
    observable->addObserver(this);

    observable_ = observable;
    RenderStage::setObservable(observable);
}

void QuadLighting::setCamera(Graph::Camera* camera)
{
    camera_ = camera;

    RenderStage::setCamera(camera);
}

bool QuadLighting::setViewport(const QRect& viewport, unsigned int samples)
{
    viewport_ = viewport;

    return RenderStage::setViewport(viewport, samples);
}

void QuadLighting::setRenderTarget(GLuint fbo)
{
    RenderStage::setRenderTarget(fbo);
    fbo_ = fbo;
}

void QuadLighting::setShadowStage(ShadowStage* shadowStage)
{
    shadowStage_ = shadowStage;
}

void QuadLighting::render()
{
    RenderStage::render();

    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    gl->glClear(GL_COLOR_BUFFER_BIT);

    if(!lightningTech_.enable())
    {
        return;
    }

    lightningTech_.setProjMatrix(camera_->projection());
    lightningTech_.setCamera(*camera_);
    lightningTech_.setViewport(viewport_);
    lightningTech_.setDepthRange(camera_->nearPlane(), camera_->farPlane());

    gbuffer_.bindTextures();
    quad_->bindVaoDirect();

    // Render directional light
    if(directionalLight_ != nullptr)
    {
        lightningTech_.enableDirectionalLight(*directionalLight_);
        quad_->renderDirect();
    }

    // Cache view matrix for positioning quads
    viewMatrix_ = camera_->view();
    viewMatrixInverse_ = viewMatrix_.inverted();

    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_ONE, GL_ONE);

    // Blend spotlights
    for(Graph::Light* light : spotLights_)
    {
        setSpotLightExtents(light);
        
        if(shadowStage_ != nullptr)
        {
            lightningTech_.enableSpotLight(*light, shadowStage_->shadowMap(light));
        }

        else
        {
            lightningTech_.enableSpotLight(*light);
        }

        quad_->renderDirect();
    }

    // Blend point lights
    for(Graph::Light* light : pointLights_)
    {
        setPointLightExtents(light);
        lightningTech_.enablePointLight(*light);

        quad_->renderDirect();
    }

    gl->glBindVertexArray(0);
    gl->glDisable(GL_BLEND);
}

void QuadLighting::visit(Graph::Light& light)
{
    switch(light.type())
    {
    case Graph::Light::LIGHT_DIRECTIONAL:
        {
            directionalLight_ = &light; break;
        }

    case Graph::Light::LIGHT_POINT:
        {
            pointLights_.push_back(&light); break;
        }

    case Graph::Light::LIGHT_SPOT:
        {
            spotLights_.push_back(&light); break;
        }
    }
}

void QuadLighting::sceneInvalidated()
{
    spotLights_.clear();
    pointLights_.clear();
    directionalLight_ = nullptr;
}

void QuadLighting::setPointLightExtents(Graph::Light* light)
{
    lightningTech_.setQuadExtents(light->cutoffDistance(),
        calcQuadPosition(light->position(), light->cutoffDistance()));
}

void QuadLighting::setSpotLightExtents(Graph::Light* light)
{
    QVector3D spotCenter = light->position() + light->direction() * light->cutoffDistance() / 2.0f;

    lightningTech_.setQuadExtents(light->cutoffDistance(),
        calcQuadPosition(spotCenter, light->cutoffDistance()));
}

QVector3D QuadLighting::calcQuadPosition(const QVector3D& lightPos, float lightCutoff)
{
    const float clipMargin = camera_->nearPlane() + 0.0001f;

    QVector3D eyePos = camera_->position() + camera_->direction() * clipMargin;
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