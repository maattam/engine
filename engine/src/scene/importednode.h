// Forms a SceneNode hierarchy from collada file (or any supported type.)

#ifndef IMPORTEDNODE_H
#define IMPORTEDNODE_H

#include "importednodedata.h"

namespace Engine {

class ImportedNode : public Resource<ImportedNode, ImportedNodeData>
{
public:
    ImportedNode();

    // Postprocess flags must be OR'ed aiProcess_* values.
    ImportedNode(const QString& name, unsigned int postprocessFlags = 0);
    virtual ~ImportedNode();

    // Attaches the loaded hierarchy to given parent
    void attach(Graph::SceneNode* parent);

    virtual ResourceDataPtr createData();

protected:
    virtual bool initialiseData(const DataType& data);
    virtual void releaseResource();

private:
    Graph::SceneNode* parentNode_;
    Graph::SceneNode* rootNode_;

    unsigned int pFlags_;

    QVector<ImportedNodeData::EntityPtr> entities_;
};

}

#endif // IMPORTEDNODE_H