// Forms a SceneNode hierarchy from collada file (or any supported type.)

#ifndef COLLADANODE_H
#define COLLADANODE_H

#include "resource.h"
#include "entity/mesh.h"

#include <vector>

class aiNode;

template<typename T>
class aiMatrix4x4t;

namespace Engine {

class ResourceDespatcher;

namespace Graph {
    class SceneNode;
}

class ColladaData : public ResourceData
{
public:
    typedef std::pair<Entity::Mesh::Ptr,
        std::vector<Entity::MeshData::SubMeshVec::size_type>> MeshIndex;

    typedef std::vector<MeshIndex> MeshIndexVec;

    ColladaData(ResourceDespatcher* despatcher);

    bool load(const QString& fileName);

    Graph::SceneNode* rootNode() const;

    Entity::MeshData::SubMeshVec::size_type numSubMeshes() const;
    const Entity::MeshData::SubMeshData& subMesh(Entity::MeshData::SubMeshVec::size_type index) const;

    Entity::MeshData::MaterialVec::size_type numMaterials() const;
    const Material::Ptr& material(Entity::MeshData::MaterialVec::size_type index) const;

    MeshIndexVec::size_type numMeshes() const;
    const MeshIndex& mesh(MeshIndexVec::size_type index) const;

private:
    MeshIndexVec meshes_;
    Entity::MeshData meshData_;

    Graph::SceneNode* rootNode_;

    void buildSceneNode(Graph::SceneNode* parent, aiNode* node, aiMatrix4x4t<float>* transform);

    // Creates new Mesh and MeshData pairing from mesh indices
    MeshIndex createMesh(unsigned int* subMeshIndex, unsigned int numMeshes, const QString& name) const;
};

class ColladaNode : public Resource<ColladaNode, ColladaData>
{
public:
    ColladaNode();
    ColladaNode(const QString& name);
    virtual ~ColladaNode();

    // Attaches the loaded hierarchy to given parent
    void attach(Graph::SceneNode* parent);

    // Finds node by name. Returns nullptr if not found.
    Graph::SceneNode* findNode(const QString& name) const;

protected:
    virtual bool initialiseData(const DataType& data);
    virtual void releaseData();

private:
    Graph::SceneNode* parentNode_;
    Graph::SceneNode* rootNode_;

    typedef std::vector<Entity::Mesh::Ptr> NodeMeshVec;
    NodeMeshVec meshes_;
};

}

#endif // COLLADANODE_H