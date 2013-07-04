#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLShaderProgram>

#include "effect/postfx.h"
#include "renderable.h"
#include "scenenode.h"

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
    QOpenGLShaderProgram program_;

    SceneNode rootNode_;

    int width_;
    int height_;

    enum { MULTISAMPLE, RESOLVE, MAX };

    GLuint framebuffer_[MAX];
    GLuint textureResolve_;

    GLuint depthRenderbuffer_;
    GLuint colorRenderbuffer_;

    GLuint quadVao_;
    GLuint quadVertexBuffer_;

    // Postprocess chain
    std::list<Postfx*> postfxChain_;

    void renderGeometry(AbstractScene* scene);
    void destroyBuffers();

    void recursiveRender(SceneNode* node, const QMatrix4x4& mvp);

    Renderer(const Renderer&);
    Renderer& operator=(const Renderer&);
};

}

#endif // RENDERER_H