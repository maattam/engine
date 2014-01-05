// Light accumulation stage using quad blending.

#ifndef QUADLIGHTING_H
#define QUADLIGHTING_H

#include "lightingstage.h"

#include "renderable/quad.h"
#include "technique/illuminationmodel.h"

#include <QVector>
#include <memory>

namespace Engine {

class ResourceDespatcher;
class GBuffer;

class QuadLighting : public LightingStage
{
public:
    QuadLighting(Renderer* renderer, GBuffer& gbuffer, ResourceDespatcher& despatcher);
    virtual ~QuadLighting();

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Graph::Camera* camera);

    // Renders the scene to a render target instead of the default surface.
    // If fbo is nullptr, the default framebuffer (0) is used.
    virtual void setRenderTarget(GLuint fbo);

    virtual void visit(Graph::Light& light);

private:
    GLuint fbo_;
    GBuffer& gbuffer_;

    QVector<Graph::Light*> spotLights_;
    QVector<Graph::Light*> pointLights_;

    Technique::IlluminationModel lightningTech_;
    std::shared_ptr<Renderable::Quad> quad_;
};

}

#endif // QUADLIGHTING_H