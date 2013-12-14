#ifndef FORWARDRENDERER_H
#define FORWARDRENDERER_H

#include "common.h"

#include "renderer.h"
#include "scene/visiblescene.h"

#include <QRect>

#include "technique/basiclightning.h"
#include "technique/shadowmap.h"
#include "renderqueue.h"
#include "material.h"

namespace Engine {

class ResourceDespatcher;

class ForwardRenderer : public Renderer
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
    // If fbo is nullptr, the default framebuffer (0) is used.
    virtual void setOutputFBO(QOpenGLFramebufferObject* fbo);

private:
    VisibleScene* scene_;
    QRect viewport_;
    unsigned int samples_;

    // Error material
    Material errorMaterial_;

    Technique::BasicLightning lightningTech_;
    Technique::ShadowMap shadowTech_;

    GLuint framebuffer_;
    GLuint renderTexture_;
    GLuint depthRenderbuffer_;

    void shadowMapPass();
    void renderPass(Entity::Camera* camera, const RenderQueue& queue);

    void renderNode(const RenderQueue::RenderItem& node);

    bool initialiseBuffers(unsigned int width, unsigned int height, unsigned int samples);
    void destroyBuffers();

    ForwardRenderer(const ForwardRenderer&);
    ForwardRenderer& operator=(const ForwardRenderer&);
};

}

#endif // FORWARDRENDERER_H