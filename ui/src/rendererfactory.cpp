//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "rendererfactory.h"

#include "deferredrenderer.h"
#include "postprocess.h"
#include "quadlighting.h"
#include "compactgbuffer.h"
#include "effect/hdr.h"
#include "technique/skybox.h"
#include "skyboxstage.h"
#include "effect/lumaexposure.h"
#include "technique/hdrtonemap.h"
#include "technique/blurfilter.h"
#include "resourcedespatcher.h"
#include "renderable/cube.h"
#include "renderable/primitive.h"
#include "forwardrenderer.h"
#include "shadowstage.h"
#include "spotlightmethod.h"

#include "rendertimewatcher.h"

#include <QDebug>

using namespace Engine;
using namespace Engine::Ui;

RendererFactory::RendererFactory(ResourceDespatcher& despatcher, RendererType type)
    : despatcher_(despatcher), type_(type), watcher_(nullptr)
{
    // HDR tonemapping
    hdrPostfx_.reset(new Effect::Hdr(&despatcher_, 4));
    hdrPostfx_->setBrightThreshold(2.0f);
    //setAutoExposure(true);

    // 5x5 Gaussian blur filter
    Effect::Hdr::BlurFilterPtr filter(new Technique::BlurFilter);
    filter->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    filter->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/gauss5x5.frag"), Shader::Type::Fragment));
    hdrPostfx_->setBlurFilter(filter);
}

Engine::GBuffer* RendererFactory::gbuffer() const
{
    return gbuffer_.get();
}

void RendererFactory::setRenderTimeWatcher(RenderTimeWatcher* watcher)
{
    watcher_ = watcher;
}

Renderer* RendererFactory::create(int samples)
{
    if(watcher_ != nullptr)
    {
        watcher_->clearStages();
    }

    gbuffer_.reset();
    Renderer* renderer = nullptr;

    if(type_ == DEFERRED)
    {
        renderer = createDeferredRenderer(samples);
    }

    else if(type_ == FORWARD)
    {
        renderer = createForwardRenderer(samples);
    }

    if(watcher_ != nullptr && !watcher_->create())
    {
        qWarning() << "Failed to create query object.";
    }

    return renderer;
}

void RendererFactory::createTonemapper(int samples)
{
    // Tonemap shader
    tonemap_.reset(new Technique::HDRTonemap(samples, 4));
    tonemap_->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));

    Shader::Ptr toneFrag = std::make_shared<Shader>(RESOURCE_PATH("shaders/postprocess.frag"), Shader::Type::Fragment);
    tonemap_->addShader(toneFrag);
    despatcher_.loadResource(toneFrag);

    hdrPostfx_->setHDRTonemapShader(tonemap_);

    tonemap_->setBloomFactor(0.25f);
    tonemap_->setBrightLevel(1.0f);
    tonemap_->setGamma(2.2f);
}

Renderer* RendererFactory::createForwardRenderer(int samples)
{
    createTonemapper(samples);

    // Skybox technique
    SkyboxStage::SkyboxPtr sky(new Technique::Skybox(samples));
    sky->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/skybox.vert"), Shader::Type::Vertex));

    Shader::Ptr skyFrag = std::make_shared<Shader>(RESOURCE_PATH("shaders/skybox.frag"), Shader::Type::Fragment);
    sky->addShader(skyFrag);
    despatcher_.loadResource(skyFrag);

    sky->setBrightness(2.0f);

    // Create multisamples fbo since we renderer directly to "screen"
    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setInternalTextureFormat(GL_RGBA16F);
    fboFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    fboFormat.setSamples(samples);

    Engine::Renderer* renderer = new ForwardRenderer(despatcher_);

    // Add skybox stage
    SkyboxStage* skybox = new Engine::SkyboxStage(renderer);
    skybox->setGBuffer(nullptr);
    skybox->setSkyboxMesh(Renderable::Primitive<Renderable::Cube>::instance());
    skybox->setSkyboxTechnique(sky);

    // Add post-process stage
    PostProcess* fxRenderer = new Engine::PostProcess(skybox, fboFormat);
    fxRenderer->setEffect(hdrPostfx_);

    if(watcher_ != nullptr)
    {
        watcher_->addRenderStage("Forward pass", skybox);
        watcher_->addRenderStage("Skybox pass", fxRenderer);
        watcher_->addNamedStage("Postprocess");
    }

    return fxRenderer;
}

Renderer* RendererFactory::createDeferredRenderer(int samples)
{
    createTonemapper(samples);

    // Skybox technique
    SkyboxStage::SkyboxPtr sky(new Technique::Skybox(samples));
    sky->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/skybox.vert"), Shader::Type::Vertex));

    Shader::Ptr skyFrag = std::make_shared<Shader>(RESOURCE_PATH("shaders/skybox.frag"), Shader::Type::Fragment);
    sky->addShader(skyFrag);
    despatcher_.loadResource(skyFrag);

    sky->setBrightness(2.0f);

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setInternalTextureFormat(GL_RGBA16F);
    fboFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    fboFormat.setSamples(1);

    // Create gbuffer
    gbuffer_.reset(new CompactGBuffer);

    DeferredRenderer* renderer = new DeferredRenderer(gbuffer_, despatcher_);
    QuadLighting* lightningStage = new QuadLighting(renderer, *gbuffer_, despatcher_, samples);

    ShadowStage* shadow = new ShadowStage(lightningStage);
    lightningStage->setShadowStage(shadow);

    // Enable spot light shadows
    ShadowStage::ShadowMethodPtr spotMethod(new SpotLightMethod(despatcher_));
    shadow->setMethod(Graph::Light::LIGHT_SPOT, spotMethod);
    shadow->createShadowMap(Graph::Light::LIGHT_SPOT, QSize(1024, 1024), 5);

    // Add skybox stage
    SkyboxStage* skybox = new Engine::SkyboxStage(shadow);
    skybox->setGBuffer(gbuffer_.get());
    skybox->setSkyboxMesh(Renderable::Primitive<Renderable::Cube>::instance());
    skybox->setSkyboxTechnique(sky);

    // Add post-process stage
    PostProcess* fxRenderer = new Engine::PostProcess(skybox, fboFormat);
    fxRenderer->setEffect(hdrPostfx_);

    if(watcher_ != nullptr)
    {
        watcher_->addRenderStage("Geometry pass", lightningStage);
        watcher_->addRenderStage("Lightning pass", shadow);
        watcher_->addRenderStage("Shadow pass", skybox);
        watcher_->addRenderStage("Skybox pass", fxRenderer);
        watcher_->addNamedStage("Postprocess");
    }

    return fxRenderer;
}

Engine::Effect::Hdr* RendererFactory::hdr() const
{
    return hdrPostfx_.get();
}

Engine::Technique::HDRTonemap* RendererFactory::tonemap() const
{
    return tonemap_.get();
}

void RendererFactory::setAutoExposure(bool value)
{
    if(value)
    {
        hdrPostfx_->setExposureFunction(std::make_shared<Effect::LumaExposure>());
    }

    else
    {
        hdrPostfx_->setExposureFunction(nullptr);
    }
}

void RendererFactory::setRendererType(RendererType type)
{
    type_ = type;
}