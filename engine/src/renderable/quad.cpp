//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "quad.h"

using namespace Engine;
using namespace Renderable;

const GLfloat QUAD_VERTEX_DATA[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};

Quad::Quad()
    : Renderable(), vertexBuffer_(0)
{
    if(!bindVertexArray())
    {
        return;
    }

    // Buffer quad
    gl->glGenBuffers(1, &vertexBuffer_);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTEX_DATA), QUAD_VERTEX_DATA, GL_STATIC_DRAW);

    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    gl->glBindVertexArray(0);

    setAABB(AABB(QVector3D(-1, -1, 0), QVector3D(1, 1, 0)));
}

Quad::~Quad()
{
    if(vertexBuffer_ != 0)
    {
        gl->glDeleteBuffers(1,  &vertexBuffer_);
    }
}

void Quad::render() const
{
    if(!bindVertexArray())
    {
        return;
    }

    renderDirect();

    gl->glBindVertexArray(0);
}

void Quad::bindVaoDirect() const
{
    bindVertexArray();
}

void Quad::renderDirect() const
{
    gl->glDrawArrays(GL_TRIANGLES, 0, 6);
}