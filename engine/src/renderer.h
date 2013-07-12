#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLShaderProgram>

#include "effect/postfx.h"
#include "scenenode.h"
#include "technique/basiclightning.h"
#include "technique/shadowmap.h"
#include "renderable/quad.h"

#include <deque>

namespace Engine {

class AbstractScene;
struct Light;
class Renderable;

class Renderer
{
public:
    explicit Renderer(QOpenGLFunctions_4_2_Core* funcs);
    ~Renderer();

    void prepareScene(AbstractScene* scene);
    void render(AbstractScene* scene);

    bool initialize(int width, int height, int samples);

private:
    typedef std::pair<QMatrix4x4, Entity::RenderList> VisibleNode;
    typedef std::deque<VisibleNode> RenderQueue;

    QOpenGLFunctions_4_2_Core* gl;

    SceneNode rootNode_;

    // TODO: Move to a separate class to allow more sophisticated culling
    RenderQueue visibles_;
    std::deque<const VisibleNode*> shadowCasters_;

    BasicLightning lightningTech_;
    ShadowMapTechnique shadowTech_;
    QOpenGLShaderProgram nullTech_;

    int width_;
    int height_;

    GLuint framebuffer_;
    GLuint renderTexture_;
    GLuint depthRenderbuffer_;

    // Postprocess chain
    std::list<Postfx*> postfxChain_;

    // Quad for postprocessing
    Quad quad_;

    void drawTextureDebug();

    void updateRenderQueue(SceneNode* node, const QMatrix4x4& worldView);
    void renderPass(AbstractScene* scene);
    void shadowMapPass(AbstractScene* scene);

    void destroyBuffers();

    Renderer(const Renderer&);
    Renderer& operator=(const Renderer&);
};

}

#endif // RENDERER_H