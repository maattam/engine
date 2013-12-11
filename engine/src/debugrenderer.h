// DebugRenderer overlays debug information resolved during the scene rendering.

#ifndef DEBUGRENDERER_H
#define DEBUGRENDERER_H

#include "renderer.h"
#include "scene/sceneobserver.h"
#include "scene/visiblescene.h"
#include "observable.h"
#include "renderable/cube.h"
#include "renderable/quad.h"
#include "entity/aabb.h"
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

    // Sets OpenGL viewport parameters.
    virtual bool setViewport(unsigned int width, unsigned int height, unsigned int samples,
        unsigned int left, unsigned int top);

    virtual bool setPostfxHook(Effect::Postfx* postfx);

    virtual void setScene(VisibleScene* scene);

    // Renders the scene through the camera's viewport.
    virtual void render(Entity::Camera* camera);

    virtual void setFlags(unsigned int flags);
    virtual unsigned int flags() const;

    // Set observable that provides debug data
    typedef Observable<SceneObserver> ObservableType;
    void setObservable(ObservableType* obs);

    // SceneObserver definitions
    virtual void beforeRendering(Entity::Entity* entity, Graph::SceneNode* node);

    // Visualize GBuffer for debugging.
    // precondition: Viewport and camera has to be the same as the debuggee's
    void setGBuffer(GBuffer* gbuffer);

private:
    QRect viewport_;
    ObservableType* observable_;
    VisibleScene* scene_;
    Entity::Camera* camera_;
    GBuffer* gbuffer_;
    unsigned int flags_;

    Renderable::Cube boundingMesh_;
    Renderable::Quad quad_;

    ShaderProgram aabbTech_;
    ShaderProgram wireframeTech_;
    Technique::GBufferVisualizer gbufferMS_;

    typedef std::pair<QMatrix4x4, QVector3D> AABBDraw;
    std::deque<AABBDraw> aabbs_;

    void renderWireframe(const RenderQueue& queue);
    void renderAABBs();
    void renderGBuffer();

    void addAABB(const QMatrix4x4& trans, const Entity::AABB& aabb, const QVector3D& color);

    DebugRenderer(const DebugRenderer&);
    DebugRenderer& operator=(const DebugRenderer&);
};

};

#endif // DEBUGRENDERER_H