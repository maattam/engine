// Interface view class for implementing different renderers.

#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"

#include <QRect>
#include <QVector>

namespace Engine {

class RenderQueue;
class CubemapTexture;

namespace Graph {
    class Camera;
    class Light;
}

class Renderer
{
public:
    virtual ~Renderer() {};

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples) = 0;

    // Sets the render queue which contains the visible geometry of the scene.
    virtual void setGeometryBatch(RenderQueue* batch) = 0;

    struct LightData
    {
        Graph::Light* light;
        RenderQueue* occluders;
    };

    // Sets lights for the current render batch.
    virtual void setLights(const QVector<LightData>& lights) = 0;

    // Sets skybox texture for the current render batch.
    virtual void setSkyboxTexture(CubemapTexture* skybox) = 0;

    // Renders the scene through the camera's viewport.
    // preconditions: batch has been set, viewport has been set, camera != nullptr
    virtual void render(Graph::Camera* camera) = 0;

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo) = 0;
};

};

#endif // RENDERER_H