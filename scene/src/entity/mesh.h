#ifndef MESH_H
#define MESH_H

#include "entity.h"
#include "renderable/submesh.h"
#include "resource.h"

#include <vector>

struct aiScene;
struct aiMesh;

namespace Engine { namespace Entity {

class MeshData : public ResourceData
{
public:
    typedef std::vector<Material::Ptr> MaterialVec;

    struct SubMeshData
    {
        std::vector<QVector3D> vertices;
        std::vector<QVector3D> normals;
        std::vector<QVector3D> tangents;
        std::vector<QVector2D> uvs;
        std::vector<unsigned int> indices;
        MaterialVec::size_type materialIndex;
        AABB aabb;
    };

    typedef std::vector<SubMeshData> SubMeshVec;

    MeshData(ResourceDespatcher* despatcher);
    MeshData(ResourceDespatcher* despatcher,
        const SubMeshVec& subMeshes, const MaterialVec& materials);

    virtual bool load(const QString& fileName);
    bool initFromScene(const aiScene* scene, const QString& fileName);

    SubMeshVec::size_type numSubMeshes() const;
    const SubMeshData& subMesh(SubMeshVec::size_type index) const;

    MaterialVec::size_type numMaterials() const;
    const Material::Ptr& material(MaterialVec::size_type index) const;

private:
    void initSubMesh(const aiMesh* mesh, SubMeshData& data);
    void initMaterials(const aiScene* scene, const QString& fileName);

    MaterialVec materials_;
    SubMeshVec meshData_;
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

    void addSubMesh(const Renderable::SubMesh::Ptr& subMesh, const AABB& aabb);
    void setMaterialAttributes(const Material::Attributes& attributes);
     
protected:
    virtual bool initialiseData(const DataType& data);
    virtual void releaseData();

private:
    std::vector<Renderable::SubMesh::Ptr> entries_;
    Material::Attributes materialAttrib_;

};

}}

#endif //MESH_H