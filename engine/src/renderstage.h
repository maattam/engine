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

    // Sets the scene to be used for future render calls
    // precondition: scene != nullptr
    virtual void setScene(VisibleScene* scene);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Entity::Camera* camera);

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