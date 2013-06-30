#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLFunctions_4_2_Core>
#include <QtGui/QOpenGLShaderProgram>
#include <QMatrix>

#include "camera.h"

namespace Engine {

class Renderer : protected QOpenGLFunctions_4_2_Core
{
public:
    explicit Renderer(QOpenGLFunctions_4_2_Core & funcs);
    ~Renderer();

    void render(const Camera& view);

    void rotateModel(float deg);

private:
    QOpenGLShaderProgram program_;

    QMatrix4x4 model_;

    GLuint vertexArrayId_;
    GLuint uvBuffer_;
    GLuint vertexBuffer_;
    GLuint textureId_;
};

}

#endif // RENDERER_H