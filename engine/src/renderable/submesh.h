// SubMesh is a vertex array object which holds indexed vertices, normals, tanges and uvs.

#ifndef SUBMESH_H
#define SUBMESH_H

#include "renderable.h"

#include <QVector>
#include <memory>

#include <QVector3D>
#include <QVector2D>

namespace Engine { namespace Renderable {

class SubMesh : public Renderable
{
public:
    typedef std::shared_ptr<SubMesh> Ptr;

    SubMesh();
    virtual ~SubMesh();

    virtual void render() const;

    bool initMesh(const QVector<QVector3D>& vertices,
                  const QVector<QVector3D>& normals,
                  const QVector<QVector3D>& tangents,
                  const QVector<QVector2D>& uvs,
                  const QVector<unsigned int>& indices);

private:
    void destroy();

    enum { BINDEX, BVERTEX, BNORMAL, BTEXCOORD, BTANGENT, NUM_BUFFERS };
    GLuint buffers_[NUM_BUFFERS];

    unsigned int numIndices_;
};

}}

#endif //MESH_H