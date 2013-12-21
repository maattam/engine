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

using namespace Engine;

RendererFactory::RendererFactory(ResourceDespatcher& despatcher)
    : despatcher_(despatcher)
{
}

Engine::GBuffer* RendererFactory::gbuffer() const
{
    return gbuffer_.get();
}

Renderer* RendererFactory::create()
{
    // HDR tonemapping
    std::shared_ptr<Effect::Hdr> hdrPostfx(new Effect::Hdr(&despatcher_, 4));
    hdrPostfx->setExposureFunction(std::make_shared<Effect::LumaExposure>());
    hdrPostfx->setBrightThreshold(1.5f);

    // 5x5 Gaussian blur filter
    Effect::Hdr::BlurFilterPtr filter(new Technique::BlurFilter);
    filter->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    filter->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/gauss5x5.frag"), Shader::Type::Fragment));
    hdrPostfx->setBlurFilter(filter);

    // Tonemap shader
    Effect::Hdr::HDRTonemapPtr tonemap(new Technique::HDRTonemap);
    tonemap->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/passthrough.vert"), Shader::Type::Vertex));
    tonemap->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/postprocess.frag"), Shader::Type::Fragment));
    hdrPostfx->setHDRTonemapShader(tonemap);

    tonemap->setBloomFactor(0.25f);
    tonemap->setBrightLevel(5.0f);
    tonemap->setGamma(2.2f);

    // Skybox technique
    SkyboxStage::SkyboxPtr sky(new Technique::Skybox);
    sky->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/skybox.vert"), Shader::Type::Vertex));
    sky->addShader(despatcher_.get<Shader>(RESOURCE_PATH("shaders/skybox.frag"), Shader::Type::Fragment));
    sky->setBrightness(5.0f);

    // GBuffer
    gbuffer_.reset(new CompactGBuffer);

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setInternalTextureFormat(GL_RGBA16F);
    fboFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    fboFormat.setSamples(1);

    DeferredRenderer* renderer = new Engine::DeferredRenderer(gbuffer_, despatcher_);
    QuadLighting* lighting = new Engine::QuadLighting(renderer, *gbuffer_.get(), despatcher_);

    SkyboxStage* skybox = new Engine::SkyboxStage(lighting);
    skybox->setGBuffer(gbuffer_.get());
    skybox->setSkyboxMesh(std::make_shared<Renderable::Cube>());
    skybox->setSkyboxTechnique(sky);

    PostProcess* fxRenderer = new Engine::PostProcess(skybox, fboFormat);
    fxRenderer->setEffect(hdrPostfx);

    return fxRenderer;
}