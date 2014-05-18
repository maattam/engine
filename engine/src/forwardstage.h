//
//  Author   : Matti Määttä
//  Summary  : Forward stage renders transparent materials using additive blending.
//

#ifndef FORWARDSTAGE_H
#define FORWARDSTAGE_H

#include "renderstage.h"
#include "renderqueue.h"
#include "technique/forwardshader.h"

#include <memory>

namespace Engine {

class GBuffer;
class ResourceDespatcher;

class ForwardStage : public RenderStage
{
public:
    explicit ForwardStage(Renderer* renderer, ResourceDespatcher& despatcher);
    virtual ~ForwardStage();

    // Sets the render queue which contains the visible geometry of the scene.
    virtual void setGeometryBatch(RenderQueue* batch);

    // Sets the camera for the current geometry batch.
    // precondition: camera != nullptr
    virtual void setCamera(Graph::Camera* camera);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render();

    // Renders the scene to a render target instead of the default surface.
    // If fbo is nullptr, the default framebuffer (0) is used.
    virtual void setRenderTarget(GLuint fbo);

    void setGBuffer(GBuffer* gbuffer);

private:
    RenderQueue* batch_;
    Graph::Camera* camera_;
    GLuint fbo_;
    GBuffer* gbuffer_;

    Technique::ForwardShader shader_;

    void renderRange(const RenderQueue::RenderRange& range);
};

}

#endif // FORWARDSTAGE