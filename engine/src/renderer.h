// Interface view class for implementing different renderers.

#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include <QRect>

namespace Engine {

class VisibleScene;

namespace Graph {
    class Camera;
}

class Renderer
{
public:
    virtual ~Renderer() {};

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples) = 0;

    // Sets the scene to be used for future render calls
    // precondition: scene != nullptr
    virtual void setScene(VisibleScene* scene) = 0;

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Graph::Camera* camera) = 0;

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo) = 0;
};

};

#endif // RENDERER_H