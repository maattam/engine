//
//  Author   : Matti Määttä
//  Summary  : DebugRenderer overlays debug information resolved during
//             the scene rendering.
//

#ifndef DEBUGRENDERER_H
#define DEBUGRENDERER_H

#include "renderer.h"
#include "scene/sceneobserver.h"
#include "renderable/cube.h"
#include "renderable/quad.h"
#include "aabb.h"
#include "shaderprogram.h"
#include "technique/gbuffervisualizer.h"

#include <QRect>
#include <deque>
#include <QVector3D>

namespace Engine {

class ResourceDespatcher;
class GBuffer;

class DebugRenderer : public Renderer, public SceneObserver
{
public:
    enum DebugFlags { DEBUG_AABB = 0x1, DEBUG_WIREFRAME = 0x2, DEBUG_GBUFFER = 0x4 };

    explicit DebugRenderer(ResourceDespatcher* despatcher);
    virtual ~DebugRenderer();

    // Sets the observable for the current scene.
    // precondition: observable != nullptr.
    virtual void setObservable(SceneObservable* observable);

    // Sets OpenGL viewport parameters.
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Sets the render queue which contains the visible geometry of the scene.
    virtual void setGeometryBatch(RenderQueue* batch);

    // Sets the camera for the current geometry batch.
    // precondition: camera != nullptr
    virtual void setCamera(Graph::Camera* camera);

    // Renders the scene through the camera's viewport.
    virtual void render();

    // Sets debugging flags
    void setFlags(unsigned int flags);
    unsigned int flags() const;

    // SceneObserver definitions
    virtual bool beforeRendering(Graph::SceneLeaf* entity, Graph::SceneNode* node);

    // Visualize GBuffer for debugging. The GBuffer is not modified.
    // precondition: Viewport and camera has to be the same as the debuggee's
    void setGBuffer(GBuffer const* gbuffer);

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo);

private:
    QRect viewport_;
    GLuint fbo_;
    GBuffer const* gbuffer_;
    unsigned int flags_;

    RenderQueue* batch_;
    Graph::Camera* camera_;

    std::shared_ptr<Renderable::Cube> boundingMesh_;
    std::shared_ptr<Renderable::Quad> quad_;

    ShaderProgram aabbTech_;
    ShaderProgram wireframeTech_;
    Technique::GBufferVisualizer gbufferMS_;

    typedef std::pair<QMatrix4x4, QVector3D> AABBDraw;
    std::deque<AABBDraw> aabbs_;

    void renderWireframe();
    void renderAABBs();
    void renderGBuffer();

    void addAABB(const QMatrix4x4& trans, const AABB& aabb, const QVector3D& color);

    DebugRenderer(const DebugRenderer&);
    DebugRenderer& operator=(const DebugRenderer&);
};

};

#endif // DEBUGRENDERER_H