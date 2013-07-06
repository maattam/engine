#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLShaderProgram>

#include "effect/postfx.h"
#include "renderable.h"
#include "scenenode.h"
#include "technique/basiclightning.h"

#include <list>

namespace Engine {

class AbstractScene;
class Light;

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

    QOpenGLFunctions_4_2_Core* gl;

    SceneNode rootNode_;
    BasicLightning technique_;

    int width_;
    int height_;

    GLuint framebuffer_;
    GLuint renderTexture_;
    GLuint depthRenderbuffer_;

    GLuint quadVao_;
    GLuint quadVertexBuffer_;

    // Postprocess chain
    std::list<Postfx*> postfxChain_;

    void renderGeometry(AbstractScene* scene);
    void destroyBuffers();

    QMatrix4x4 VP_;
    void recursiveRender(SceneNode* node, const QMatrix4x4& worldView);

    Renderer(const Renderer&);
    Renderer& operator=(const Renderer&);
};

}

#endif // RENDERER_H