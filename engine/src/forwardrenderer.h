#ifndef FORWARDRENDERER_H
#define FORWARDRENDERER_H

#include "common.h"

#include "renderer.h"
#include "scene/visiblescene.h"

#include <QRect>

#include "technique/basiclightning.h"
#include "technique/shadowmap.h"
#include "technique/skyboxtechnique.h"
#include "renderable/quad.h"
#include "renderable/cube.h"
#include "material.h"

#include <deque>
#include <memory>

namespace Engine {

class ResourceDespatcher;

namespace Effect {
    class Postfx;
}

class ForwardRenderer : public Renderer
{
public:
    enum RenderFlags { RENDER_SHADOWS = 0x1 };

    explicit ForwardRenderer(ResourceDespatcher* despatcher);
    virtual ~ForwardRenderer();

    // Sets OpenGL viewport parameters.
    virtual bool setViewport(unsigned int width, unsigned int height, unsigned int samples,
        unsigned int left, unsigned int top);

    virtual void setScene(VisibleScene* scene);

    // Renders the scene through the camera's viewport.
    virtual void render(Entity::Camera* camera);

    virtual void setFlags(unsigned int flags);
    virtual unsigned int flags() const;

private:
    VisibleScene* scene_;
    unsigned int flags_;
    QRect viewport_;

    // Error material
    Material errorMaterial_;

    Technique::BasicLightning lightningTech_;
    Technique::ShadowMap shadowTech_;
    Technique::Skybox skyboxTech_;

    // For debugging depth buffer
    QOpenGLShaderProgram nullTech_;

    GLuint framebuffer_;
    GLuint renderTexture_;
    GLuint depthRenderbuffer_;

    // Postprocess chain
    std::list<Effect::Postfx*> postfxChain_;

    // Quad for postprocessing
    Renderable::Quad quad_;

    void shadowMapPass();
    void renderPass(Entity::Camera* camera, const RenderQueue& queue);
    void skyboxPass(Entity::Camera* camera);

    void renderNode(const RenderQueue::RenderItem& node);

    bool initialiseBuffers(unsigned int width, unsigned int height, unsigned int samples);
    void destroyBuffers();

    ForwardRenderer(const ForwardRenderer&);
    ForwardRenderer& operator=(const ForwardRenderer&);
};

}

#endif // FORWARDRENDERER_H