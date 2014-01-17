//
//  Author   : Matti Määttä
//  Summary  : A set of factory functions which create SceneLeaves from Assimp structures.
//

#ifndef NODEIMPORT_H
#define NODEIMPORT_H

#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QString>

#include "aabb.h"

#include <memory>

struct aiMesh;
struct aiMaterial;
struct aiCamera;
struct aiLight;

namespace Engine {

namespace Graph {
    class Camera;
    class Light;
}

class Material;
class ResourceDespatcher;

namespace NodeImport {

typedef std::shared_ptr<Graph::Camera> CameraPtr;
typedef std::shared_ptr<Graph::Light> LightPtr;
typedef std::shared_ptr<Material> MaterialPtr;

struct ImportException
{
    explicit ImportException(const QString& msg) : message(msg) {};

    QString message;
};

struct IndexMesh
{
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector3D> tangents;
    QVector<QVector2D> uvs;
    QVector<unsigned int> indices;

    unsigned int materialIndex;
    AABB aabb;
};

// Imports index meshes from the scene.
// precondition: meshes != nullptr, count is the mesh count
// throws: ImportException
void importMeshes(QVector<IndexMesh>& indexVec, aiMesh** meshes, unsigned int count);

// Imports materials. rootDir should be the root directory for textures.
// precondition: materials != nullptr, count is the material count
// throws: ImportException
QVector<MaterialPtr> importMaterials(aiMaterial** materials, unsigned int count,
                                     ResourceDespatcher* despatcher, const QString& rootDir);

// Imports cameras
// precondition: cameras != nullptr, count is the camera count
// throws: ImportException
QVector<CameraPtr> importCameras(aiCamera** cameras, unsigned int count);

// Imports lights
// precondition: lights != nullptr, count is the light count
// throws: ImportException
QVector<LightPtr> importLights(aiLight** lights, unsigned int count);

}}

#endif // NODEIMPORT_H