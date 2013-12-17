// Forms a SceneNode hierarchy from collada file (or any supported type.)

#ifndef IMPORTEDNODE_H
#define IMPORTEDNODE_H

#include "importednodedata.h"

namespace Engine {

class ImportedNode : public Resource<ImportedNode, ImportedNodeData>
{
public:
    ImportedNode();
    ImportedNode(const QString& name);
    virtual ~ImportedNode();

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

    QVector<ImportedNodeData::EntityPtr> entities_;
};

}

#endif // IMPORTEDNODE_H