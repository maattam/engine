// Rudimentary Assimp loader library

#ifndef SUBMESH_H
#define SUBMESH_H

#include "renderable.h"

#include <string>
#include <vector>
#include <memory>

#include <QVector3D>
#include <QVector2D>

class aiScene;
class aiMesh;

namespace Engine {

class SubMesh : public Renderable
{
public:
    typedef std::shared_ptr<SubMesh> Ptr;

    explicit SubMesh(QOpenGLFunctions_4_2_Core* funcs);
    ~SubMesh();

    void render();

    bool initMesh(  const std::vector<QVector3D>& vertices,
                    const std::vector<QVector3D>& normals,
                    const std::vector<QVector2D>& uvs,
                    const std::vector<unsigned int>& indices);

private:
    void destroy();

    enum { BINDEX, BVERTEX, BNORMAL, BTEXCOORD, NUM_BUFFERS };
    GLuint buffers_[NUM_BUFFERS];

    unsigned int numIndices_;
};

}

#endif //MESH_H