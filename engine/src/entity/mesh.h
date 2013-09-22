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
    Mesh(const QString& name);
    ~Mesh();

    virtual void updateRenderList(RenderList& list);

    size_t numSubMeshes() const;
    const Renderable::SubMesh::Ptr& subMesh(size_t index) const;

    void setMaterialAttributes(const Material::Attributes& attributes);

protected:
    bool loadData(const QString& fileName);
    bool initializeData();

private:
    bool initFromScene(const aiScene* scene, const QString& filenName);

    struct MeshData
    {
        std::vector<QVector3D> vertices;
        std::vector<QVector3D> normals;
        std::vector<QVector3D> tangents;
        std::vector<QVector2D> uvs;
        std::vector<unsigned int> indices;
        unsigned int materialIndex;
    };

    void initSubMesh(const aiMesh* mesh, MeshData* data);

    void initMaterials(const aiScene* scene, const QString& fileName);

    std::vector<Renderable::SubMesh::Ptr> entries_;
    std::vector<Material::Ptr> materials_;
    Material::Attributes materialAttrib_;
    std::vector<std::shared_ptr<MeshData>> meshData_;
};

}}

#endif //MESH_H