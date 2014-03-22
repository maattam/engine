//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "importednode.h"

#include "graph/scenenode.h"
#include "graph/geometry.h"
#include "renderable/mesh.h"
#include "scene/scenemanager.h"

#include <QVector>

using namespace Engine;

ImportedNode::ImportedNode(SceneManager& scene)
    : Resource(), rootNode_(nullptr), parentNode_(nullptr), scene_(scene), pFlags_(0)
{
}

ImportedNode::ImportedNode(const QString& name, SceneManager& scene, unsigned int postprocessFlags)
    : Resource(name, ResourceBase::QUEUED), rootNode_(nullptr), parentNode_(nullptr), pFlags_(postprocessFlags), scene_(scene)
{
}

ImportedNode::~ImportedNode()
{
    // Remove entities from scene
    for(ImportedNodeData::EntityPtr& leaf : entities_)
    {
        scene_.removeSceneLeaf(leaf);
    }

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
            parentNode_->removeChild(rootNode_.get());
        }

        parentNode_ = parent;
        parentNode_->addChild(rootNode_.get());
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

    rootNode_.reset(data.rootNode());

    // Copy entities
    entities_ = data.entities();

    // Make IndexMeshes into Meshes
    QVector<Graph::Geometry::Ptr> subMeshes;
    subMeshes.resize(data.indexMeshes().count());

    for(int i = 0; i < data.indexMeshes().count(); ++i)
    {
        const NodeImport::IndexMesh& mesh = data.indexMeshes().at(i);
        Renderable::Mesh::Ptr subMesh = std::make_shared<Renderable::Mesh>();
        subMesh->setAABB(mesh.aabb);

        if(!subMesh->initMesh(mesh.vertices, mesh.normals,
                mesh.tangents, mesh.uvs, mesh.indices))
        {
            return false;
        }

        subMeshes[i] = std::make_shared<Graph::Geometry>(subMesh, data.materials().at(mesh.materialIndex));
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

            entity->attach(meshIndices.node);
            entities_.push_back(entity);
        }
    }

    // Attach to parent if set
    if(parentNode_ != nullptr)
    {
        parentNode_->addChild(rootNode_.get());
    }

    // Add entities to scene
    for(ImportedNodeData::EntityPtr& leaf : entities_)
    {
        scene_.addSceneLeaf(leaf);
    }

    return true;
}

void ImportedNode::releaseResource()
{
    if(parentNode_ != nullptr)
    {
        parentNode_->removeChild(rootNode_.get());
    }

    rootNode_.reset();
    entities_.clear();
}

ImportedNode::ResourceDataPtr ImportedNode::createData()
{
    std::shared_ptr<ImportedNodeData> data(new ImportedNodeData);
    data->setPostprocessFlags(pFlags_);

    return data;
}

ImportedNode::Ptr ImportedNode::clone() const
{
    ImportedNode::Ptr node(new ImportedNode(scene_));
    node->parentNode_ = parentNode_;

    // Copy root node
    node->rootNode_ = rootNode_;

    // Clone entities
    for(const ImportedNodeData::EntityPtr& leaf : entities_)
    {
        node->entities_.push_back(Graph::SceneLeaf::clone(leaf));
    }

    return node;
}