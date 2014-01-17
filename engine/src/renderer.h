//
//  Author   : Matti Määttä
//  Summary  : Interface class for implementing different renderers.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"

#include <QRect>
#include <QVector>

namespace Engine {

class RenderQueue;
class SceneObservable;

namespace Graph {
    class Camera;
}

class Renderer
{
public:
    virtual ~Renderer() {};

    // Sets the observable for the current scene.
    // precondition: observable != nullptr.
    virtual void setObservable(SceneObservable* observable) = 0;

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples) = 0;

    // Sets the render queue which contains the visible geometry of the scene.
    // precondition: batch != nullptr
    virtual void setGeometryBatch(RenderQueue* batch) = 0;

    // Sets the camera for the current geometry batch.
    // This function is called once per frame before calling render() and before
    // the visible leaves are resolved.
    // precondition: camera != nullptr
    virtual void setCamera(Graph::Camera* camera) = 0;

    // Renders the scene through the camera's viewport.
    // preconditions: batch has been set, viewport has been set, camera has been set.
    virtual void render() = 0;

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo) = 0;
};

};

#endif // RENDERER_H