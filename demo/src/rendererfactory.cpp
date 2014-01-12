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
#include "forwardrenderer.h"

#include "rendertimewatcher.h"

using namespace Engine;

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
    watcher_->clearStages();

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

    // Skybox technique
    SkyboxStage::SkyboxPtr sky(new Technique::Skybox(samples));
    sky->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/skybox.vert"), Shader::Type::Vertex));

    Shader::Ptr skyFrag = std::make_shared<Shader>(RESOURCE_PATH("shaders/skybox.frag"), Shader::Type::Fragment);
    sky->addShader(skyFrag);
    despatcher_.loadResource(skyFrag);

    sky->setBrightness(2.0f);

    gbuffer_.reset();

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setInternalTextureFormat(GL_RGBA16F);

    Engine::Renderer* renderer = nullptr;

    if(type_ == DEFERRED)
    {
        // GBuffer
        gbuffer_.reset(new CompactGBuffer);

        fboFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
        fboFormat.setSamples(1);

        DeferredRenderer* deferred = new DeferredRenderer(gbuffer_, despatcher_);
        RenderStage* lightningStage = new QuadLighting(deferred, *gbuffer_.get(), despatcher_, samples);
        watcher_->addRenderStage("Geometry pass", lightningStage);

        renderer = lightningStage;
    }

    else
    {
        fboFormat.setAttachment(QOpenGLFramebufferObject::Depth);
        fboFormat.setSamples(samples);

        renderer = new ForwardRenderer(despatcher_);
    }

    SkyboxStage* skybox = new Engine::SkyboxStage(renderer);
    watcher_->addRenderStage("Lightning pass", skybox);

    skybox->setGBuffer(gbuffer_.get());
    skybox->setSkyboxMesh(std::make_shared<Renderable::Cube>());
    skybox->setSkyboxTechnique(sky);

    PostProcess* fxRenderer = new Engine::PostProcess(skybox, fboFormat);
    watcher_->addRenderStage("Skybox pass", fxRenderer);
    fxRenderer->setEffect(hdrPostfx_);

    watcher_->addNamedStage("Postprocess");
    watcher_->create();

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