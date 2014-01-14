// QuadLightning renders screen-aligned quads representing light enclosures and additively blends
// resulting light into accumulation buffer.

#ifndef QUADLIGHTING_H
#define QUADLIGHTING_H

#include "renderstage.h"
#include "visitor.h"
#include "scene/sceneobserver.h"

#include "renderable/quad.h"
#include "technique/illuminationmodel.h"

#include <QVector>
#include <memory>

namespace Engine {

class ResourceDespatcher;
class GBuffer;

class QuadLighting : public RenderStage, public SceneObserver,
    public BaseVisitor, public Visitor<Graph::Light>
{
public:
    QuadLighting(Renderer* renderer, GBuffer& gbuffer, ResourceDespatcher& despatcher, unsigned int samples);
    virtual ~QuadLighting();

    // Sets the observable for the current scene.
    // precondition: observable != nullptr.
    virtual void setObservable(SceneObservable* observable);

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Sets the camera for the current geometry batch.
    // precondition: camera != nullptr
    virtual void setCamera(Graph::Camera* camera);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render();

    // Renders the scene to a render target instead of the default surface.
    // If fbo is nullptr, the default framebuffer (0) is used.
    virtual void setRenderTarget(GLuint fbo);

    virtual void visit(Graph::Light& light);

    virtual void sceneInvalidated();

private:
    GLuint fbo_;
    GBuffer& gbuffer_;
    QRect viewport_;

    QVector<Graph::Light*> spotLights_;
    QVector<Graph::Light*> pointLights_;
    Graph::Light* directionalLight_;
    Graph::Camera* camera_;
    SceneObservable* observable_;

    Technique::IlluminationModel lightningTech_;
    std::shared_ptr<Renderable::Quad> quad_;

    // Cached values
    QMatrix4x4 viewMatrix_;
    QMatrix4x4 viewMatrixInverse_;

    void setPointLightExtents(Graph::Light* light);
    void setSpotLightExtents(Graph::Light* light);

    // Positions light so it faces the camera at the light's minZ extent.
    QVector3D calcQuadPosition(const QVector3D& lightPos, float lightCutoff);
};

}

#endif // QUADLIGHTING_H