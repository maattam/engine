#include "renderer.h"

#include <QDebug>

using namespace Engine;

const GLfloat VERTEX_BUFFER_DATA[] = {
    -0.8f, -0.8f, 0.0f,
    0.8f, -0.8f, 0.0f,
    0.0f, 0.8f, 0.0f };

Renderer::Renderer(QOpenGLContext* context) : context_(context)
{
    GLenum err = glewInit();

    if(err == GLEW_OK)
    {
        qDebug() << "Initialized Renderer: OpenGL version " << context_->format().majorVersion()
            << "." << context_->format().minorVersion();
    }

    else
    {
        qDebug() << "Failed to initialize OpenGL: " << glewGetErrorString(err);
        return;
    }

    glGenVertexArrays(1, &vertexArrayId_);
    glBindVertexArray(vertexArrayId_);

    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_BUFFER_DATA), VERTEX_BUFFER_DATA, GL_STATIC_DRAW);

    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/triangle.vert");
    program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/triangle.frag");
    program_.link();
}

Renderer::~Renderer()
{
}

void Renderer::render()
{
    program_.bind();

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);

    program_.release();
}