// OffscreenRenderer implements a simple forward pass using an anynomous technique.
// This renderer is used by shadow and cubemap renderers which need more simplified pipeline
// than actual renderers that output directly to screen.

#ifndef OFFSCREENRENDERER_H
#define OFFSCREENRENDERER_H

#include "renderer.h"
#include "renderqueue.h"

#include <functional>

namespace Engine {

namespace Technique {
    class Technique;
}

class Material;

class OffscreenRenderer : public Renderer
{
public:
    OffscreenRenderer();
    virtual ~OffscreenRenderer();

    // Sets the observable for the current scene.
    // precondition: observable != nullptr.
    virtual void setObservable(SceneObservable* observable);

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Sets the render queue which contains the visible geometry of the scene.
    // precondition: batch != nullptr
    virtual void setGeometryBatch(RenderQueue* batch);

    // Sets the camera for the current geometry batch.
    // This function is called once per frame before calling render() and before
    // the visible leaves are resolved.
    // precondition: camera != nullptr
    virtual void setCamera(Graph::Camera* camera);

    // Renders the scene through the camera's viewport.
    // preconditions: batch has been set, viewport has been set, camera has been set.
    virtual void render();

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo);

    // Technique used for rendering batch.
    void setTechnique(Technique::Technique* tech);

    typedef std::function<void(const Material&, const QMatrix4x4&)> OnRenderCallback;

    // OnRenderCallback is called before each renderable is rendered.
    // This allows the caller to set technique's attributes for the current material.
    void setRenderCallback(OnRenderCallback callback);

private:
    RenderQueue* batch_;
    Graph::Camera* camera_;
    Technique::Technique* tech_;

    GLuint fbo_;
    OnRenderCallback renderCallback_;

    QRect viewport_;

    void renderBatch(const RenderQueue::RenderRange& range);
};

};

#endif // OFFSCREENRENDERER_H