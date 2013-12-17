#include "importednode.h"

#include "graph/scenenode.h"
#include "entity/subentity.h"
#include "entity/mesh.h"

#include <QVector>

using namespace Engine;

ImportedNode::ImportedNode()
    : Resource(), rootNode_(nullptr), parentNode_(nullptr)
{
}

ImportedNode::ImportedNode(const QString& name)
    : Resource(name, ResourceBase::QUEUED), rootNode_(nullptr), parentNode_(nullptr)
{
}

ImportedNode::~ImportedNode()
{
    ImportedNode::releaseData();
}

void ImportedNode::attach(Graph::SceneNode* parent)
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

Graph::SceneNode* ImportedNode::findNode(const QString& name) const
{
    return nullptr; // TODO
}

bool ImportedNode::initialiseData(const DataType& data)
{
    if(data.rootNode() == nullptr)
        return false;

    rootNode_ = data.rootNode();

    // Copy entities
    entities_ = data.entities();

    // Make IndexMeshes into SubMeshes
    QVector<Entity::SubEntity::Ptr> subMeshes;
    subMeshes.resize(data.indexMeshes().count());

    for(int i = 0; i < data.indexMeshes().count(); ++i)
    {
        const NodeImport::IndexMesh& mesh = data.indexMeshes().at(i);
        Renderable::SubMesh::Ptr subMesh = std::make_shared<Renderable::SubMesh>();

        if(!subMesh->initMesh(mesh.vertices, mesh.normals,
                mesh.tangents, mesh.uvs, mesh.indices))
        {
            return false;
        }

        subMeshes[i] = std::make_shared<Entity::SubEntity>(subMesh,
            data.materials().at(mesh.materialIndex), mesh.aabb);
    }

    // Group submesh data to meshes
    for(const auto& meshIndices : data.meshIndices())
    {
        const ImportedNodeData::MeshPtr& mesh = meshIndices.first;
        const DataType::MeshIndex::second_type& indexVec = meshIndices.second;

        // Sanity checks
        Q_ASSERT(!indexVec.empty());

        for(auto it = indexVec.begin(); it != indexVec.end(); ++it)
        {
            Q_ASSERT(*it < data.indexMeshes().count());
            mesh->addSubEntity(subMeshes.at(*it));
        }

        entities_.push_back(mesh);
    }

    // Attach to parent if set
    if(parentNode_ != nullptr)
    {
        parentNode_->addChild(rootNode_);
    }

    return true;
}

void ImportedNode::releaseData()
{
    if(parentNode_ != nullptr)
    {
        parentNode_->removeChild(rootNode_);
    }

    delete rootNode_;
    rootNode_ = nullptr;

    entities_.clear();
}