#ifndef RENDERERFACTORY_H
#define RENDERERFACTORY_H

namespace Engine {
    class ResourceDespatcher;
    class Renderer;
    class GBuffer;
}

#include <memory>

class RendererFactory
{
public:
    explicit RendererFactory(Engine::ResourceDespatcher& despatcher);

    Engine::Renderer* create();

    Engine::GBuffer* gbuffer() const;

private:
    Engine::ResourceDespatcher& despatcher_;

    std::shared_ptr<Engine::GBuffer> gbuffer_;
};

#endif // RENDERERFACTORY_H