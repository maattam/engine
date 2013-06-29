#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>

namespace Engine {

class Renderer
{
public:
    Renderer(QOpenGLContext* context = nullptr);
    ~Renderer();

    void render();

private:
    QOpenGLContext* context_;

    QOpenGLShaderProgram program_;

    GLuint vertexArrayId_;
    GLuint vertexBuffer_;
};

}

#endif // RENDERER_H