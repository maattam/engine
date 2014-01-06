#include "mesh.h"

#include <QDebug>

using namespace Engine;
using namespace Renderable;

Mesh::Mesh()
    : Renderable(), numIndices_(0)
{
    for(int i = 0; i < NUM_BUFFERS; ++i)
        buffers_[i] = 0;
}

Mesh::~Mesh()
{
    destroy();
}

void Mesh::destroy()
{
    gl->glDeleteBuffers(NUM_BUFFERS, buffers_);

    for(int i = 0; i < NUM_BUFFERS; ++i)
        buffers_[i] = 0;

    numIndices_ = 0;
}

void Mesh::render() const
{
    if(!bindVertexArray() || numIndices_ == 0)
        return;

    gl->glDrawElements(GL_TRIANGLES, numIndices_, GL_UNSIGNED_INT, 0);

    gl->glBindVertexArray(0);
}

bool Mesh::initMesh( const QVector<QVector3D>& vertices,
                        const QVector<QVector3D>& normals,
                        const QVector<QVector3D>& tangents,
                        const QVector<QVector2D>& uvs,
                        const QVector<unsigned int>& indices)
{
    destroy();

    // Allocate and populate buffers
    if(!bindVertexArray())
        return false;

    gl->glGenBuffers(NUM_BUFFERS, buffers_);

    // Vertex buffer
    gl->glBindBuffer(GL_ARRAY_BUFFER, buffers_[BVERTEX]);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(ATTRIB_VERTICES);
    gl->glVertexAttribPointer(ATTRIB_VERTICES, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UV buffer
    gl->glBindBuffer(GL_ARRAY_BUFFER, buffers_[BTEXCOORD]);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(uvs[0]) * uvs.size(), &uvs[0], GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(ATTRIB_TEXCOORDS);
    gl->glVertexAttribPointer(ATTRIB_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Normal buffer
    gl->glBindBuffer(GL_ARRAY_BUFFER, buffers_[BNORMAL]);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(ATTRIB_NORMALS);
    gl->glVertexAttribPointer(ATTRIB_NORMALS, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Tangent buffer
    if(tangents.size() > 0)
    {
        gl->glBindBuffer(GL_ARRAY_BUFFER, buffers_[BTANGENT]);
        gl->glBufferData(GL_ARRAY_BUFFER, sizeof(tangents[0]) * tangents.size(), &tangents[0], GL_STATIC_DRAW);
        gl->glEnableVertexAttribArray(ATTRIB_TANGENTS);
        gl->glVertexAttribPointer(ATTRIB_TANGENTS, 3, GL_FLOAT, GL_FALSE, 0, 0);

        setTangents(true);
    }

    // Index buffer
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BINDEX]);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    gl->glBindVertexArray(0);

    numIndices_ = indices.size();

    return true;
}