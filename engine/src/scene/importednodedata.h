#ifndef IMPORTEDNODEDATA_H
#define IMPORTEDNODEDATA_H

#include "resource.h"
#include "nodeimport.h"
#include "resourcedata.h"

#include <QMap>
#include <QList>
#include <vector>

struct aiNode;

template<typename T>
class aiMatrix4x4t;

namespace Engine {

class ResourceDespatcher;

namespace Graph {
    class SceneNode;
}

namespace Entity {
    class Entity;
}

class ImportedNodeData : public ResourceData
{
public:
    struct MeshIndex
    {
        Graph::SceneNode* node;
        QString name;
        std::vector<unsigned int> meshIndices;
    };

    typedef std::shared_ptr<Entity::Entity> EntityPtr;

    ImportedNodeData();

    bool load(const QString& fileName);

    // Returns the populated SceneNode graph
    Graph::SceneNode* rootNode() const;

    const QVector<MeshIndex>& meshIndices() const;
    const QVector<NodeImport::MaterialPtr>& materials() const;
    const QVector<NodeImport::IndexMesh>& indexMeshes() const;

    // Returns imported entities such as lights and cameras
    const QVector<EntityPtr>& entities() const;

    // OR's flags with aiProcess_* flags before loading data.
    void setPostprocessFlags(unsigned int flags);

private:
    QVector<MeshIndex> meshIndices_;
    QVector<EntityPtr> entities_;
    QVector<NodeImport::MaterialPtr> materials_;
    QVector<NodeImport::IndexMesh> indexMeshes_;

    unsigned int pFlags_;
    Graph::SceneNode* rootNode_;

    void buildSceneNode(Graph::SceneNode* parent, aiNode* node, aiMatrix4x4t<float>* transform);

    // Creates new Mesh and MeshData pairing from mesh indices
    MeshIndex createMesh(Graph::SceneNode* node, unsigned int* subMeshIndex, unsigned int numMeshes, const QString& name) const;

    QList<Entity::Entity*> findEntities(const QString& name);
};

}

#endif // IMPORTEDNODEDATA_H