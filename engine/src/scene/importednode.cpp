#include "importednode.h"

#include "graph/scenenode.h"
#include "graph/geometry.h"
#include "renderable/submesh.h"

#include <QVector>

using namespace Engine;

ImportedNode::ImportedNode()
    : Resource(), rootNode_(nullptr), parentNode_(nullptr)
{
}

ImportedNode::ImportedNode(const QString& name, unsigned int postprocessFlags)
    : Resource(name, ResourceBase::QUEUED), rootNode_(nullptr), parentNode_(nullptr), pFlags_(postprocessFlags)
{
}

ImportedNode::~ImportedNode()
{
    ImportedNode::releaseResource();
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

bool ImportedNode::initialiseData(const DataType& data)
{
    if(data.rootNode() == nullptr)
        return false;

    rootNode_ = data.rootNode();

    // Copy entities
    entities_ = data.entities();

    // Make IndexMeshes into SubMeshes
    QVector<Graph::Geometry::Ptr> subMeshes;
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

        subMeshes[i] = std::make_shared<Graph::Geometry>(subMesh,
            data.materials().at(mesh.materialIndex), mesh.aabb);
    }

    // Group submesh data to meshes
    for(const auto& meshIndices : data.meshIndices())
    {
        const std::vector<unsigned int>& indexVec = meshIndices.meshIndices;

        // Sanity checks
        Q_ASSERT(!indexVec.empty());

        for(auto it = indexVec.begin(); it != indexVec.end(); ++it)
        {
            Q_ASSERT(*it < data.indexMeshes().count());

            // Attach SubEntity to the assigned to form the complete mesh.
            Graph::Geometry::Ptr& entity = subMeshes[*it];
            entity->setName(meshIndices.name);

            meshIndices.node->attachEntity(entity.get());
            entities_.push_back(entity);
        }
    }

    // Attach to parent if set
    if(parentNode_ != nullptr)
    {
        parentNode_->addChild(rootNode_);
    }

    return true;
}

void ImportedNode::releaseResource()
{
    if(parentNode_ != nullptr)
    {
        parentNode_->removeChild(rootNode_);
    }

    delete rootNode_;
    rootNode_ = nullptr;

    entities_.clear();
}

ImportedNode::ResourceDataPtr ImportedNode::createData()
{
    std::shared_ptr<ImportedNodeData> data(new ImportedNodeData);
    data->setPostprocessFlags(pFlags_);

    return data;
}