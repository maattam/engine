#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLShaderProgram>

#include "effect/postfx.h"
#include "scenenode.h"
#include "technique/basiclightning.h"
#include "technique/shadowmap.h"
#include "renderable/quad.h"

#include <list>
#include <vector>

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
    typedef std::list<Renderable*> RenderList;
    typedef std::vector<std::pair<QMatrix4x4, RenderList>> RenderQueue;

    QOpenGLFunctions_4_2_Core* gl;

    SceneNode rootNode_;
    RenderQueue renderQueue_;

    BasicLightning lightningTech_;
    ShadowMapTechnique shadowTech_;
    QOpenGLShaderProgram nullTech_;

    int width_;
    int height_;

    GLuint framebuffer_;
    GLuint renderTexture_;
    GLuint depthRenderbuffer_;

    QMatrix4x4 lightVP_;

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