// Forms a SceneNode hierarchy from collada file (or any supported type.)

#ifndef IMPORTEDNODE_H
#define IMPORTEDNODE_H

#include "importednodedata.h"

namespace Engine {

class SceneManager;

class ImportedNode : public Resource<ImportedNode, ImportedNodeData>
{
public:
    explicit ImportedNode(SceneManager& manager);

    // Postprocess flags must be OR'ed aiProcess_* values.
    ImportedNode(const QString& name, SceneManager& scene, unsigned int postprocessFlags = 0);
    virtual ~ImportedNode();

    // Attaches the loaded hierarchy to given parent
    void attach(Graph::SceneNode* parent);

    virtual ResourceDataPtr createData();

    // Creates a copy of the ImportedNode and all entities.
    // The resulting copy is no longer attached to the resource despatcher.
    // Clone should be made after the resource has been loaded.
    // TODO: The underlying SceneNode is not copied.
    ImportedNode::Ptr clone() const;

    template<typename LeafType>
    std::shared_ptr<LeafType> findLeaf(const QString& name);

protected:
    virtual bool initialiseData(const DataType& data);
    virtual void releaseResource();

private:
    Graph::SceneNode* parentNode_;
    std::shared_ptr<Graph::SceneNode> rootNode_;
    SceneManager& scene_;

    unsigned int pFlags_;

    QVector<ImportedNodeData::EntityPtr> entities_;
};

#include "importednode.inl"

}

#endif // IMPORTEDNODE_H