// Mesh is a collection of SubEntities, and represents an object that as been split into
// several submeshes.

#ifndef MESH_H
#define MESH_H

#include "entity.h"
#include "subentity.h"
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
    typedef std::vector<SubEntity::Ptr> SubEntityVec;

public:
    Mesh();
    Mesh(const QString& name);
    ~Mesh();

    virtual void updateRenderList(RenderQueue& list);

    void addSubEntity(const SubEntity::Ptr& subEntity);

    SubEntityVec::size_type numSubEntities() const;
    const SubEntity::Ptr& subEntity(SubEntityVec::size_type index);
     
protected:
    virtual bool initialiseData(const DataType& data);
    virtual void releaseData();

private:
    SubEntityVec entries_;

};

}}

#endif //MESH_H