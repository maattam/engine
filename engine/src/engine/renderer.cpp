#include "renderer.h"

#include <QImage>
#include <QMatrix>
#include <QVector>
#include <QDebug>

using namespace Engine;

const GLfloat VERTEX_BUFFER_DATA[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

const GLfloat UV_BUFFER_DATA[] = {
    0.000059f, 1.0f-0.000004f,
    0.000103f, 1.0f-0.336048f,
    0.335973f, 1.0f-0.335903f,
    1.000023f, 1.0f-0.000013f,
    0.667979f, 1.0f-0.335851f,
    0.999958f, 1.0f-0.336064f,
    0.667979f, 1.0f-0.335851f,
    0.336024f, 1.0f-0.671877f,
    0.667969f, 1.0f-0.671889f,
    1.000023f, 1.0f-0.000013f,
    0.668104f, 1.0f-0.000013f,
    0.667979f, 1.0f-0.335851f,
    0.000059f, 1.0f-0.000004f,
    0.335973f, 1.0f-0.335903f,
    0.336098f, 1.0f-0.000071f,
    0.667979f, 1.0f-0.335851f,
    0.335973f, 1.0f-0.335903f,
    0.336024f, 1.0f-0.671877f,
    1.000004f, 1.0f-0.671847f,
    0.999958f, 1.0f-0.336064f,
    0.667979f, 1.0f-0.335851f,
    0.668104f, 1.0f-0.000013f,
    0.335973f, 1.0f-0.335903f,
    0.667979f, 1.0f-0.335851f,
    0.335973f, 1.0f-0.335903f,
    0.668104f, 1.0f-0.000013f,
    0.336098f, 1.0f-0.000071f,
    0.000103f, 1.0f-0.336048f,
    0.000004f, 1.0f-0.671870f,
    0.336024f, 1.0f-0.671877f,
    0.000103f, 1.0f-0.336048f,
    0.336024f, 1.0f-0.671877f,
    0.335973f, 1.0f-0.335903f,
    0.667969f, 1.0f-0.671889f,
    1.000004f, 1.0f-0.671847f,
    0.667979f, 1.0f-0.335851f
};

Renderer::Renderer(QOpenGLContext* context) : context_(context)
{
    glewExperimental = GL_TRUE;
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

    /*glGenVertexArrays(1, &uvArrayId_);
    glBindVertexArray(uvArrayId_);

    glGenBuffers(1, &uvBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UV_BUFFER_DATA), UV_BUFFER_DATA, GL_STATIC_DRAW);*/

    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/triangle.vert");
    program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/triangle.frag");
    program_.link();

    /*QImage texture;
    if(! texture.load(":/images/wooden_crate.png"))
    {
        qDebug() << "Failed to load texture";
    }

    else
    {
        texture = texture.convertToFormat(QImage::Format_ARGB32);

        glGenTextures(1, &textureId_);
        glBindTexture(GL_TEXTURE_2D, textureId_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.width(), texture.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, texture.bits());
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);*/
}

Renderer::~Renderer()
{
}

void Renderer::resize(int width, int height)
{
    QMatrix4x4 projection, view;

    projection.perspective(45.0f, static_cast<float>(width) / height, 0.1f, 100.0f);
    view.lookAt(QVector3D(5, 3, -5), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    model_.rotate(3.0f, 1.0f, 0.0f);

    // model projection
    mvp_ = projection * view * model_;
}

void Renderer::render()
{
    program_.bind();
    program_.setUniformValueArray("mvp", &mvp_, 1);
    //program_.setUniformValue("tex", textureId_);

    program_.enableAttributeArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    program_.disableAttributeArray(0);

    /*program_.enableAttributeArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    program_.disableAttributeArray(1);*/

    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, textureId_);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDrawArrays(GL_TRIANGLES, 0, 12*3);

    program_.release();
}
