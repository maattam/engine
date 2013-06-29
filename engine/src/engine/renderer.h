#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QMatrix>

namespace Engine {

class Renderer
{
public:
    Renderer(QOpenGLContext* context = nullptr);
    ~Renderer();

    void render();
    void resize(int width, int height);

private:
    QOpenGLContext* context_;
    QOpenGLShaderProgram program_;

    QMatrix4x4 model_;
    QMatrix4x4 mvp_;
    GLuint vertexArrayId_;
    GLuint uvArrayId_;
    GLuint uvBuffer_;
    GLuint vertexBuffer_;
    GLuint textureId_;
};

}

#endif // RENDERER_H