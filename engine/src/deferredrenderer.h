#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "renderer.h"

#include "technique/dsgeometryshader.h"
#include "material.h"
#include "renderqueue.h"

#include <QRect>

namespace Engine {

class ResourceDespatcher;
class GBuffer;

class DeferredRenderer : public Renderer
{
public:
    typedef std::shared_ptr<GBuffer> GBufferPtr;

    // GBuffer ownership is maintained
    DeferredRenderer(const GBufferPtr& gbuffer, ResourceDespatcher& despatcher);
    virtual ~DeferredRenderer();

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Sets the scene to be used for future render calls
    // precondition: scene != nullptr
    virtual void setScene(VisibleScene* scene);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Graph::Camera* camera);

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo);

private:
    GBufferPtr gbuffer_;
    QRect viewport_;

    VisibleScene* scene_;
    Graph::Camera* camera_;

    RenderQueue renderQueue_;
    Technique::DSGeometryShader geometryShader_;

    // Error material
    Material errorMaterial_;

    bool initialise(unsigned int width, unsigned int height, unsigned int samples);

    void geometryPass();

    DeferredRenderer(const DeferredRenderer&);
    DeferredRenderer& operator=(const DeferredRenderer&);
};

}

#endif // DEFERREDRENDERER_H