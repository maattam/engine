#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "renderer.h"

#include "technique/dsgeometryshader.h"
#include "technique/dsmaterialshader.h"
#include "renderable/quad.h"
#include "material.h"

#include <QRect>

namespace Engine {

class ResourceDespatcher;
class GBuffer;
class RenderQueue;

class DeferredRenderer : public Renderer
{
public:
    DeferredRenderer(GBuffer& gbuffer, ResourceDespatcher& despatcher);
    virtual ~DeferredRenderer();

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Sets the scene to be used for future render calls
    // precondition: scene != nullptr
    virtual void setScene(VisibleScene* scene);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Entity::Camera* camera);

    // TODO: Replace flags with a proper configuration interface
    virtual void setFlags(unsigned int flags);
    virtual unsigned int flags() const;

    // Sets and initialises the GBuffer.
    // precondition: Viewport has been set
    // postcondition: The associated buffer is maintained by the renderer.
    void setGBuffer(GBuffer& gbuffer);

    // Renders the scene to a render target instead of the default surface.
    // If fbo is nullptr, the default framebuffer (0) is used.
    virtual void setOutputFBO(QOpenGLFramebufferObject* fbo);

private:
    GBuffer& gbuffer_;
    QRect viewport_;
    unsigned int samples_;

    QOpenGLFramebufferObject* output_;
    VisibleScene* scene_;
    Entity::Camera* camera_;

    Technique::DSGeometryShader geometryShader_;
    Technique::DSMaterialShader materialShader_;

    // Error material
    Material errorMaterial_;

    // Quad for screen-space rendering
    Renderable::Quad quad_;

    bool initialise(unsigned int width, unsigned int height, unsigned int samples);

    void geometryPass(const RenderQueue& queue);
    void preLightPass();

    DeferredRenderer(const DeferredRenderer&);
    DeferredRenderer& operator=(const DeferredRenderer&);
};

}

#endif // DEFERREDRENDERER_H