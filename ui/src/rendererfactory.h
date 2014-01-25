//
//  Author   : Matti Määttä
//  Summary  : 
//

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

namespace Ui {

class RenderTimeWatcher;

class RendererFactory
{
public:
    enum RendererType { DEFERRED, FORWARD };

    RendererFactory(ResourceDespatcher& despatcher, RendererType type = DEFERRED);

    void setRendererType(RendererType type);

    Renderer* create(int samples);

    GBuffer* gbuffer() const;
    Effect::Hdr* hdr() const;
    Technique::HDRTonemap* tonemap() const;

    void setAutoExposure(bool value);

    void setRenderTimeWatcher(RenderTimeWatcher* watcher);

private:
    Engine::ResourceDespatcher& despatcher_;
    RendererType type_;

    RenderTimeWatcher* watcher_;

    std::shared_ptr<GBuffer> gbuffer_;
    std::shared_ptr<Technique::HDRTonemap> tonemap_;
    std::shared_ptr<Effect::Hdr> hdrPostfx_;

    void createTonemapper(int samples);

    Engine::Renderer* createForwardRenderer(int samples);
    Engine::Renderer* createDeferredRenderer(int samples);
};

}}

#endif // RENDERERFACTORY_H