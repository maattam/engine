#ifndef RENDERERFACTORY_H
#define RENDERERFACTORY_H

namespace Engine {
    class ResourceDespatcher;
    class Renderer;
    class GBuffer;
    
    namespace Technique {
        class HDRTonemap;
    }

    namespace Effect {
        class Hdr;
    }
}

#include <memory>

class RendererFactory
{
public:
    enum RendererType { DEFERRED, FORWARD };

    RendererFactory(Engine::ResourceDespatcher& despatcher, RendererType type = DEFERRED);

    void setRendererType(RendererType type);

    Engine::Renderer* create(int samples);

    Engine::GBuffer* gbuffer() const;
    Engine::Effect::Hdr* hdr() const;
    Engine::Technique::HDRTonemap* tonemap() const;

    void setAutoExposure(bool value);

private:
    Engine::ResourceDespatcher& despatcher_;
    RendererType type_;

    std::shared_ptr<Engine::GBuffer> gbuffer_;
    std::shared_ptr<Engine::Technique::HDRTonemap> tonemap_;
    std::shared_ptr<Engine::Effect::Hdr> hdrPostfx_;
};

#endif // RENDERERFACTORY_H