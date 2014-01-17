//
//  Author   : Matti Määttä
//  Summary  : The renderer can be extended using the decorator pattern.
//             RenderStage is the decorator interface for all renderer decorators.
//

#ifndef RENDERSTAGE_H
#define RENDERSTAGE_H

#include "renderer.h"

#include <QObject>

namespace Engine {

class RenderStage : public QObject, public Renderer
{
    Q_OBJECT

public:
    // RenderStage takes ownership of the renderer and handles deallocation
    // Invariant: renderer != nullptr, not deleted outside
    explicit RenderStage(Renderer* renderer);
    virtual ~RenderStage();

    // Sets the observable for the current scene.
    // precondition: observable != nullptr.
    virtual void setObservable(SceneObservable* observable);
    
    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Sets the render queue which contains the visible geometry of the scene.
    virtual void setGeometryBatch(RenderQueue* batch);

    // Sets the camera for the current geometry batch.
    // precondition: camera != nullptr
    virtual void setCamera(Graph::Camera* camera);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render();

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo);

signals:
    // Emitted when the previous stage has finished rendering.
    void stageFinished();

private:
    Renderer* renderer_;

    RenderStage(const RenderStage&);
    RenderStage& operator=(const RenderStage&);
};

}

#endif // RENDERSTAGE_H