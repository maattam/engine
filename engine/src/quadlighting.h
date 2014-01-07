// QuadLightning renders screen-aligned quads representing light enclosures and additively blends
// resulting light into accumulation buffer.

#ifndef QUADLIGHTING_H
#define QUADLIGHTING_H

#include "renderstage.h"

#include "renderable/quad.h"
#include "technique/illuminationmodel.h"

#include <QVector>
#include <memory>

namespace Engine {

class ResourceDespatcher;
class GBuffer;

class QuadLighting : public RenderStage
{
public:
    QuadLighting(Renderer* renderer, GBuffer& gbuffer, ResourceDespatcher& despatcher);
    virtual ~QuadLighting();

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Sets lights for the current render batch.
    virtual void setLights(const QVector<LightData>& lights);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Graph::Camera* camera);

    // Renders the scene to a render target instead of the default surface.
    // If fbo is nullptr, the default framebuffer (0) is used.
    virtual void setRenderTarget(GLuint fbo);

private:
    GLuint fbo_;
    GBuffer& gbuffer_;
    QRect viewport_;

    QVector<Graph::Light*> spotLights_;
    QVector<Graph::Light*> pointLights_;
    Graph::Light* directionalLight_;

    ShaderProgram testQuad_;

    Technique::IlluminationModel lightningTech_;
    std::shared_ptr<Renderable::Quad> quad_;

    void setPointLightExtents(Graph::Camera* camera, Graph::Light* light);
};

}

#endif // QUADLIGHTING_H