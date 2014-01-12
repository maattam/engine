#ifndef RENDERERFACTORY_H
#define RENDERERFACTORY_H

#include <memory>

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

class RenderTimeWatcher;

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

    void setRenderTimeWatcher(RenderTimeWatcher* watcher);

private:
    Engine::ResourceDespatcher& despatcher_;
    RendererType type_;

    RenderTimeWatcher* watcher_;

    std::shared_ptr<Engine::GBuffer> gbuffer_;
    std::shared_ptr<Engine::Technique::HDRTonemap> tonemap_;
    std::shared_ptr<Engine::Effect::Hdr> hdrPostfx_;
};

#endif // RENDERERFACTORY_H