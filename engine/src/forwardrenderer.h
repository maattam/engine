#ifndef FORWARDRENDERER_H
#define FORWARDRENDERER_H

#include "common.h"

#include "renderer.h"
#include "visitor.h"
#include "scene/sceneobserver.h"

#include <QRect>

#include "technique/basiclightning.h"
#include "renderqueue.h"
#include "material.h"

namespace Engine {

namespace Graph {
    class Light;
}

class ResourceDespatcher;

class ForwardRenderer : public Renderer, public SceneObserver,
    public BaseVisitor, public Visitor<Graph::Light>
{
public:
    explicit ForwardRenderer(ResourceDespatcher& despatcher);
    virtual ~ForwardRenderer();

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

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo);

    virtual void visit(Graph::Light& light);

    virtual void sceneInvalidated();

private:
    RenderQueue* renderQueue_;
    QRect viewport_;
    unsigned int samples_;

    // Error material
    Material errorMaterial_;

    Technique::BasicLightning lightningTech_;

    GLuint fbo_;

    Graph::Light* directionalLight_;
    QList<Graph::Light*> lights_;
    Graph::Camera* camera_;
    SceneObservable* observable_;

    //void shadowMapPass();
    void renderPass();

    void renderRange(RenderQueue::RenderRange range);
    void renderNode(const RenderQueue::RenderItem& node);

    ForwardRenderer(const ForwardRenderer&);
    ForwardRenderer& operator=(const ForwardRenderer&);
};

}

#endif // FORWARDRENDERER_H