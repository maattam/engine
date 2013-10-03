#include "colladanode.h"

#include "graph/scenenode.h"

#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace Engine;

namespace {
    QMatrix4x4 aiMatrixToQMatrix(const aiMatrix4x4& mat);
}

//
// ColladaNode
//

ColladaNode::ColladaNode()
    : Resource(), rootNode_(nullptr), parentNode_(nullptr)
{
}

ColladaNode::ColladaNode(const QString& name)
    : Resource(name, ResourceBase::QUEUED), rootNode_(nullptr), parentNode_(nullptr)
{
}

ColladaNode::~ColladaNode()
{
    ColladaNode::releaseData();
}

void ColladaNode::attach(Graph::SceneNode* parent)
{
    if(parent == nullptr)
        return;

    if(rootNode_ != nullptr)
    {
        // Detach from old parent
        if(parentNode_ != nullptr)
        {
            parentNode_->removeChild(rootNode_);
        }

        parentNode_ = parent;
        parentNode_->addChild(rootNode_);
    }

    // Queue attachment
    else
    {
        parentNode_ = parent;
    }
}

Graph::SceneNode* ColladaNode::findNode(const QString& name) const
{
    return nullptr; // TODO
}

bool ColladaNode::initialiseData(const DataType& data)
{
    if(data.rootNode() == nullptr)
        return false;

    rootNode_ = data.rootNode();

    // Upload submeshes
    std::vector<Renderable::SubMesh::Ptr> subMeshes;
    subMeshes.resize(data.numSubMeshes());

    for(int i = 0; i < data.numSubMeshes(); ++i)
    {
        const Entity::MeshData::SubMeshData& mesh = data.subMesh(i);

        subMeshes[i] = std::make_shared<Renderable::SubMesh>();
        subMeshes[i]->setMaterial(data.material(mesh.materialIndex));

        if(!subMeshes[i]->initMesh(mesh.vertices, mesh.normals,
                mesh.tangents, mesh.uvs, mesh.indices))
        {
            return false;
        }
    }

    // Group submesh data to meshes
    for(int i = 0; i < data.numMeshes(); ++i)
    {
        const Entity::Mesh::Ptr& mesh = data.mesh(i).first;
        const DataType::MeshIndex::second_type& indexVec = data.mesh(i).second;

        // Sanity checks
        assert(!indexVec.empty());

        for(auto it = indexVec.begin(); it != indexVec.end(); ++it)
        {
            assert(*it < data.numSubMeshes());
            mesh->addSubMesh(subMeshes.at(*it), data.subMesh(*it).aabb);
        }

        meshes_.push_back(mesh);
    }

    // Attach to parent if set
    if(parentNode_ != nullptr)
    {
        parentNode_->addChild(rootNode_);
    }

    return true;
}

void ColladaNode::releaseData()
{
    if(parentNode_ != nullptr)
    {
        parentNode_->removeChild(rootNode_);
    }

    delete rootNode_;
    rootNode_ = nullptr;

    meshes_.clear();
}

//
// ColladaData
//

ColladaData::ColladaData(ResourceDespatcher* despatcher)
    : ResourceData(despatcher), meshData_(despatcher)
{
}

bool ColladaData::load(const QString& fileName)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(fileName.toStdString(),
        aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);

    if(scene == nullptr)
    {
        qWarning() << __FUNCTION__ << "Error loading '" << fileName << "': " << importer.GetErrorString();
        return false;
    }

    // Read all submeshes and materials into a MeshData structure
    if(!meshData_.initFromScene(scene, fileName))
    {
        return false;
    }

    // Read the mesh hierarchy recursively
    aiMatrix4x4 identity;
    rootNode_ = new Graph::SceneNode;
    buildSceneNode(rootNode_, scene->mRootNode, &identity);

    return true;
}

void ColladaData::buildSceneNode(Graph::SceneNode* parent, aiNode* node, aiMatrix4x4* accTransform)
{
    aiMatrix4x4 transform = (*accTransform) * node->mTransformation;
    Graph::SceneNode* newParent = parent;

    // If node has meshes, create new scenenode and attach Mesh to it
    if(node->mNumMeshes > 0)
    {
        MeshIndex meshIndex = createMesh(node->mMeshes, node->mNumMeshes, node->mName.C_Str());
        meshes_.push_back(meshIndex);

        // Copy node transformation over
        QMatrix4x4 nodeTransf = aiMatrixToQMatrix(transform);

        // Create new node
        newParent = dynamic_cast<Graph::SceneNode*>(parent->createChild(nodeTransf));
        newParent->attachEntity(meshIndex.first.get());

        // Don't carry transformation over to child nodes
        transform = aiMatrix4x4();
    }

    // Build all child nodes
    for(unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        buildSceneNode(newParent, node->mChildren[i], &transform);
    }
}

ColladaData::MeshIndex ColladaData::createMesh(unsigned int* subMeshIndex,
                                               unsigned int numMeshes, const QString& name) const
{
    // Copy mesh indices
    MeshIndex::second_type indices(subMeshIndex, subMeshIndex + numMeshes);
    Entity::Mesh::Ptr mesh = std::make_shared<Entity::Mesh>(name);

    return std::make_pair(mesh, indices);
}

Graph::SceneNode* ColladaData::rootNode() const
{
    return rootNode_;
}

Entity::MeshData::SubMeshVec::size_type ColladaData::numSubMeshes() const
{
    return meshData_.numSubMeshes();
}

const Entity::MeshData::SubMeshData& ColladaData::subMesh(Entity::MeshData::SubMeshVec::size_type index) const
{
    return meshData_.subMesh(index);
}

Entity::MeshData::MaterialVec::size_type ColladaData::numMaterials() const
{
    return meshData_.numMaterials();
}

const Material::Ptr& ColladaData::material(Entity::MeshData::MaterialVec::size_type index) const
{
    return meshData_.material(index);
}

ColladaData::MeshIndexVec::size_type ColladaData::numMeshes() const
{
    return meshes_.size();
}

const ColladaData::MeshIndex& ColladaData::mesh(MeshIndexVec::size_type index) const
{
    return meshes_.at(index);
}

namespace {

    QMatrix4x4 aiMatrixToQMatrix(const aiMatrix4x4& mat)
    {
        // aiMatrix4x4 is row major
        return QMatrix4x4(
            mat.a1, mat.a2, mat.a3, mat.a4,
            mat.b1, mat.b2, mat.b3, mat.b4,
            mat.c1, mat.c2, mat.c3, mat.c4,
            mat.d1, mat.d2, mat.d3, mat.d4);
    }

}