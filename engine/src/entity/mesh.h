#ifndef MESH_H
#define MESH_H

#include "entity.h"
#include "renderable/submesh.h"
#include "resource.h"

#include <vector>
#include <memory>

struct aiScene;
struct aiMesh;

namespace Engine { namespace Entity {

class Mesh : public Entity, public Resource
{
public:
    typedef std::shared_ptr<Mesh> Ptr;

    Mesh();
    ~Mesh();

    bool load(const QString& fileName);

    virtual void updateRenderList(RenderList& list);

    void setMaterialAttributes(const Material::Attributes& attributes);

private:
    bool initFromScene(const aiScene* scene, const QString& filenName);

    void initSubMesh(const aiMesh* mesh,
        std::vector<QVector3D>& vertices,
        std::vector<QVector3D>& normals,
        std::vector<QVector3D>& tangents,
        std::vector<QVector2D>& uvs,
        std::vector<unsigned int>& indices);

    void initMaterials(const aiScene* scene, const QString& fileName);

    std::vector<Renderable::SubMesh::Ptr> entries_;
    std::vector<Material::Ptr> materials_;
};

}}

#endif //MESH_H