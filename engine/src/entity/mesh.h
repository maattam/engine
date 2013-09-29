#ifndef MESH_H
#define MESH_H

#include "entity.h"
#include "renderable/submesh.h"
#include "resource/resource.h"

#include <vector>

struct aiScene;
struct aiMesh;

namespace Engine { namespace Entity {

class MeshData : public ResourceData
{
public:
    MeshData(ResourceDespatcher* despatcher);

    virtual bool load(const QString& fileName);

    struct SubMeshData
    {
        std::vector<QVector3D> vertices;
        std::vector<QVector3D> normals;
        std::vector<QVector3D> tangents;
        std::vector<QVector2D> uvs;
        std::vector<unsigned int> indices;
        unsigned int materialIndex;
    };

    std::vector<Material::Ptr>& materials();
    std::vector<SubMeshData>& meshes();
    const AABB& aabb() const;

private:
    bool initFromScene(const aiScene* scene, const QString& fileName);
    void initSubMesh(const aiMesh* mesh, SubMeshData& data);
    void initMaterials(const aiScene* scene, const QString& fileName);

    std::vector<Material::Ptr> materials_;
    std::vector<SubMeshData> meshData_;
    AABB aabb_;
};

class Mesh : public Entity, public Resource<Mesh, MeshData>
{
public:
    Mesh();
    Mesh(const QString& name);
    ~Mesh();

    virtual void updateRenderList(RenderList& list);

    size_t numSubMeshes() const;
    const Renderable::SubMesh::Ptr& subMesh(size_t index) const;

    void setMaterialAttributes(const Material::Attributes& attributes);
     
protected:
    virtual bool initialiseData(DataType& data);
    virtual void releaseData();

private:
    std::vector<Renderable::SubMesh::Ptr> entries_;
    Material::Attributes materialAttrib_;

};

}}

#endif //MESH_H