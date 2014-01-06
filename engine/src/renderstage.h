// The renderer can be extended using the decorator pattern.
// RenderStage is the decorator interface for all renderer decorators.

#ifndef RENDERSTAGE_H
#define RENDERSTAGE_H

#include "renderer.h"

namespace Engine {

class RenderStage : public Renderer
{
public:
    // RenderStage takes ownership of the renderer and handles deallocation
    // Invariant: renderer != nullptr, not deleted outside
    explicit RenderStage(Renderer* renderer);
    virtual ~RenderStage();
    
    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Sets the render queue which contains the visible geometry of the scene.
    virtual void setGeometryBatch(RenderQueue* batch);

    // Sets lights for the current render batch.
    virtual void setLights(const QVector<LightData>& lights);

    // Sets skybox texture for the current render batch.
    virtual void setSkyboxTexture(CubemapTexture* skybox);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Graph::Camera* camera);

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo);

private:
    Renderer* renderer_;

    RenderStage(const RenderStage&);
    RenderStage& operator=(const RenderStage&);
};

}

#endif // RENDERSTAGE_H