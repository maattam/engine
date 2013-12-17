#include "importednodedata.h"

#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "graph/scenenode.h"
#include "entity/mesh.h"
#include "entity/camera.h"
#include "entity/light.h"

#include <QDebug>
#include <QDir>

using namespace Engine;

namespace {
    QMatrix4x4 aiMatrixToQMatrix(const aiMatrix4x4& mat);
}

ImportedNodeData::ImportedNodeData(ResourceDespatcher* despatcher)
    : ResourceData(despatcher)
{
}

bool ImportedNodeData::load(const QString& fileName)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(fileName.toStdString(),
        aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);

    if(scene == nullptr)
    {
        qWarning() << __FUNCTION__ << "Error loading '" << fileName << "': " << importer.GetErrorString();
        return false;
    }

    QString dir = QFileInfo(fileName).dir().path();

    // Import entities from scene
    try
    {
        indexMeshes_ = NodeImport::importMeshes(scene->mMeshes, scene->mNumMeshes);
        materials_ = NodeImport::importMaterials(scene->mMaterials, scene->mNumMaterials, despatcher(), dir);

        for(auto& entity : NodeImport::importCameras(scene->mCameras, scene->mNumCameras))
        {
            entities_.push_back(entity);
        }

        for(auto& entity : NodeImport::importLights(scene->mLights, scene->mNumLights))
        {
            entities_.push_back(entity);
        }
    }

    catch(NodeImport::ImportException& ex)
    {
        qDebug() << "Failed to import" << fileName << ":" << ex.message;
        return false;
    }

    // Read the mesh hierarchy recursively
    aiMatrix4x4 identity;
    rootNode_ = new Graph::SceneNode;
    buildSceneNode(rootNode_, scene->mRootNode, &identity);

    return true;
}

void ImportedNodeData::buildSceneNode(Graph::SceneNode* parent, aiNode* node, aiMatrix4x4* accTransform)
{
    aiMatrix4x4 transform = (*accTransform) * node->mTransformation;
    Graph::SceneNode* newParent = parent;

    // If node is named, save transformation
    if(node->mName.length > 0 || node->mNumMeshes > 0)
    {
        // Copy node transformation over
        QMatrix4x4 nodeTransf = aiMatrixToQMatrix(transform);

        // Create new node
        newParent = parent->createSceneNodeChild();
        newParent->applyTransformation(nodeTransf);

        // Don't carry transformation over to child nodes
        transform = aiMatrix4x4();
    }

    if(node->mName.length > 0)
    {
        for(Entity::Entity* entity : findEntities(node->mName.data))
        {
            newParent->attachEntity(entity);
        }
    }

    // If node has meshes, create new scenenode and attach Mesh to it
    if(node->mNumMeshes > 0)
    {
        MeshIndex meshIndex = createMesh(node->mMeshes, node->mNumMeshes, node->mName.C_Str());
        meshIndices_.push_back(meshIndex);

        newParent->attachEntity(meshIndex.first.get());
    }

    // Build all child nodes
    for(unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        buildSceneNode(newParent, node->mChildren[i], &transform);
    }
}

ImportedNodeData::MeshIndex ImportedNodeData::createMesh(unsigned int* subMeshIndex,
                                               unsigned int numMeshes, const QString& name) const
{
    // Copy mesh indices
    MeshIndex::second_type indices(subMeshIndex, subMeshIndex + numMeshes);
    MeshPtr mesh(new Entity::Mesh);
    mesh->setName(name);

    return std::make_pair(mesh, indices);
}

QList<Entity::Entity*> ImportedNodeData::findEntities(const QString& name)
{
    QList<Entity::Entity*> entities;

    for(const EntityPtr& entity : entities_)
    {
        if(entity->name() == name)
        {
            entities.push_back(entity.get());
        }
    }

    return entities;
}

Graph::SceneNode* ImportedNodeData::rootNode() const
{
    return rootNode_;
}

const QVector<ImportedNodeData::MeshIndex>& ImportedNodeData::meshIndices() const
{
    return meshIndices_;
}

const QVector<ImportedNodeData::EntityPtr>& ImportedNodeData::entities() const
{
    return entities_;
}

const QVector<NodeImport::MaterialPtr>& ImportedNodeData::materials() const
{
    return materials_;
}

const QVector<NodeImport::IndexMesh>& ImportedNodeData::indexMeshes() const
{
    return indexMeshes_;
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