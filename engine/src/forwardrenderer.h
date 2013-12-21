#ifndef FORWARDRENDERER_H
#define FORWARDRENDERER_H

#include "common.h"

#include "renderer.h"

#include <QRect>
#include <QList>

#include "technique/basiclightning.h"
#include "technique/shadowmap.h"
#include "renderqueue.h"
#include "material.h"
#include "visitor.h"

namespace Engine {

namespace Entity {
    class Light;
}

class ResourceDespatcher;

class ForwardRenderer : public Renderer,
    public BaseVisitor, public Visitor<Entity::Light>
{
public:
    explicit ForwardRenderer(ResourceDespatcher& despatcher);
    virtual ~ForwardRenderer();

    // Sets OpenGL viewport parameters.
    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    virtual void setScene(VisibleScene* scene);

    // Renders the scene through the camera's viewport.
    virtual void render(Entity::Camera* camera);

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo);

    virtual void visit(Entity::Light& light);

private:
    VisibleScene* scene_;
    QRect viewport_;
    unsigned int samples_;

    // Error material
    Material errorMaterial_;

    Technique::BasicLightning lightningTech_;
    Technique::ShadowMap shadowTech_;

    GLuint fbo_;

    QList<Entity::Light*> lights_;

    void shadowMapPass();
    void renderPass(Entity::Camera* camera, const RenderQueue& queue);
    void renderNode(const RenderQueue::RenderItem& node);

    bool initialiseBuffers(unsigned int width, unsigned int height, unsigned int samples);

    ForwardRenderer(const ForwardRenderer&);
    ForwardRenderer& operator=(const ForwardRenderer&);
};

}

#endif // FORWARDRENDERER_H