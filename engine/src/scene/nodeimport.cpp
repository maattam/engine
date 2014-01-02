#include "nodeimport.h"

#include "resourcedespatcher.h"
#include "material.h"
#include "entity/camera.h"
#include "entity/light.h"
#include "textureloader.h"
#include "texture2dresource.h"
#include "mathelp.h"

#include <assimp/scene.h>

#include <QDebug>

using namespace Engine;
using namespace Engine::NodeImport;

namespace {
    void importMesh(aiMesh* mesh, IndexMesh& indexMesh);
    void importMaterial(aiMaterial* aiMat, Material& material, const QString& rootDir, ResourceDespatcher* despatcher);
    void importCamera(aiCamera* aiCam, CameraPtr& entity);
    void importLight(aiLight* light, LightPtr& entity);
    void initMaterialAttributes(aiMaterial* mat, Material& target);
}

// Imports index meshes from the scene.
// precondition: meshes != nullptr, count is the mesh count
// throws: ImportException
void NodeImport::importMeshes(QVector<IndexMesh>& indexMeshes, aiMesh** meshes, unsigned int count)
{
    indexMeshes.clear();
    indexMeshes.resize(count);

    for(unsigned int i = 0; i < count; ++i)
    {
        importMesh(meshes[i], indexMeshes[i]);
    }
}

// Imports materials
// precondition: materials != nullptr, count is the material count
// throws: ImportException
QVector<MaterialPtr> NodeImport::importMaterials(aiMaterial** materials, unsigned int count,
                                     ResourceDespatcher* despatcher, const QString& rootDir)
{
    QVector<MaterialPtr> materialVec;
    materialVec.resize(count);

    for(unsigned int i = 0; i < count; ++i)
    {
        MaterialPtr& material = materialVec[i];
        material.reset(new Material());

        importMaterial(materials[i], *material, rootDir, despatcher);
    }

    return materialVec;
}

// Imports cameras
// precondition: cameras != nullptr, count is the camera count
// throws: ImportException
QVector<CameraPtr> NodeImport::importCameras(aiCamera** cameras, unsigned int count)
{
    QVector<CameraPtr> cameraVec;
    cameraVec.resize(count);

    for(unsigned int i = 0; i < count; ++i)
    {
        importCamera(cameras[i], cameraVec[i]);
    }

    return cameraVec;
}

// Imports lights
// precondition: lights != nullptr, count is the light count
// throws: ImportException
QVector<LightPtr> NodeImport::importLights(aiLight** lights, unsigned int count)
{
    QVector<LightPtr> lightVec;
    lightVec.resize(count);

    for(unsigned int i = 0; i < count; ++i)
    {
        importLight(lights[i], lightVec[i]);
    }

    return lightVec;
}

namespace {

void importMesh(aiMesh* mesh, IndexMesh& indexMesh)
{
    const aiVector3D zero3D(0, 0, 0);

    // Fill vertex attribute vectors
    for(size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& pos       = mesh->mVertices[i];
        const aiVector3D& normal    = mesh->mNormals[i];
        const aiVector3D& uv        = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero3D;

        QVector3D vertice(pos.x, pos.y, pos.z);
        indexMesh.vertices.push_back(vertice);
        indexMesh.normals.push_back(QVector3D(normal.x, normal.y, normal.z));

        if(mesh->HasTangentsAndBitangents())
        {
            const aiVector3D& tangent   = mesh->mTangents[i];
            indexMesh.tangents.push_back(QVector3D(tangent.x, tangent.y, tangent.z));
        }

        indexMesh.uvs.push_back(QVector2D(uv.x, uv.y));
        indexMesh.materialIndex = mesh->mMaterialIndex;

        // Form bounding rect
        indexMesh.aabb.resize(vertice);
    }

    // Fill the index buffer
    for(size_t i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        Q_ASSERT(face.mNumIndices == 3);

        indexMesh.indices.push_back(face.mIndices[0]);
        indexMesh.indices.push_back(face.mIndices[1]);
        indexMesh.indices.push_back(face.mIndices[2]);
    }
}

void importMaterial(aiMaterial* aiMat, Material& material, const QString& rootDir, ResourceDespatcher* despatcher)
{
    // Create mapping between Material::TextureType, TextureConversion and aiTextureType
    const std::pair<aiTextureType, TextureConversion> textureMapping[Material::TEXTURE_COUNT] = {
        std::make_pair(aiTextureType_DIFFUSE, TC_SRGBA),
        std::make_pair(aiTextureType_NORMALS, TC_RGBA),
        std::make_pair(aiTextureType_SPECULAR, TC_GRAYSCALE),
        std::make_pair(aiTextureType_OPACITY, TC_GRAYSCALE)
    };

    aiString path;
    QString fullpath = rootDir + "/";

    // Query supported materials
    for(int j = 0; j < Material::TEXTURE_COUNT; ++j)
    {
        if(aiMat->GetTextureCount(textureMapping[j].first) > 0 &&
            aiMat->GetTexture(textureMapping[j].first, 0, &path) == aiReturn_SUCCESS)
        {
            const Material::TextureType type = static_cast<Material::TextureType>(j);
            Material::TexturePtr texture = despatcher->get<Texture2DResource>(fullpath + path.data,
                textureMapping[j].second);

            material.setTexture(type, texture);
        }
    }

    // Hack: Workaround for broken waveform normals
    if(aiMat->GetTextureCount(aiTextureType_HEIGHT) > 0 &&
        aiMat->GetTexture(aiTextureType_HEIGHT, 0, &path) == aiReturn_SUCCESS)
    {
        Material::TexturePtr texture = despatcher->get<Texture2DResource>(fullpath + path.data);
        material.setTexture(Material::TEXTURE_NORMALS, texture);
    }

    initMaterialAttributes(aiMat, material);
}

void importCamera(aiCamera* aiCam, CameraPtr& entity)
{
    // aiCamera only supports perspective projection
    entity.reset(new Entity::Camera(Entity::Camera::PERSPECTIVE));

    entity->setName(aiCam->mName.data);
    entity->setPosition(QVector3D(aiCam->mPosition.x, aiCam->mPosition.y, aiCam->mPosition.z));
    entity->setNearPlane(aiCam->mClipPlaneNear);
    entity->setFarPlane(aiCam->mClipPlaneFar);
    entity->setFov(aiCam->mHorizontalFOV);
    entity->setAspectRatio(aiCam->mAspect);
    entity->setDirection(QVector3D(aiCam->mLookAt.x, aiCam->mLookAt.y, aiCam->mLookAt.z));

    QVector3D up = QVector3D(aiCam->mUp.x, aiCam->mUp.y, aiCam->mUp.z).normalized();
    QVector3D lookAt = QVector3D(aiCam->mLookAt.x, aiCam->mLookAt.y, aiCam->mLookAt.z).normalized();
    QVector3D right = QVector3D::crossProduct(up, lookAt);

    entity->setOrientation(orientationFromAxes(right, up, lookAt));
}

void importLight(aiLight* light, LightPtr& entity)
{
    Entity::Light::LightType lightType;

    switch(light->mType)
    {
    case aiLightSource_DIRECTIONAL: lightType = Entity::Light::LIGHT_DIRECTIONAL; break;
    case aiLightSource_POINT: lightType = Entity::Light::LIGHT_POINT; break;
    case aiLightSource_SPOT: lightType = Entity::Light::LIGHT_SPOT;  break;

    default: qDebug() << "Import light: undefined light type, ignored."; return;
    }

    entity.reset(new Entity::Light(lightType));
    entity->setName(light->mName.data);

    // Set colors
    // TODO: Specular color
    entity->setColor(QVector3D(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b));

    // Set direction for directional and spot lights
    entity->setDirection(QVector3D(light->mDirection.x, light->mDirection.y, light->mDirection.z));

    // Set attenuation
    entity->setAttenuationConstant(light->mAttenuationConstant);
    entity->setAttenuationLinear(light->mAttenuationLinear);
    entity->setAttenuationQuadratic(light->mAttenuationQuadratic);

    entity->setAngleOuterCone(light->mAngleOuterCone);
    entity->setAngleInnerCone(light->mAngleInnerCone);
    entity->setPosition(QVector3D(light->mPosition.x, light->mPosition.y, light->mPosition.z));
}

void initMaterialAttributes(aiMaterial* mat, Material& target)
{
    aiColor3D color;

    // If color map texture was not set, check if attribute exists instead
    if(mat->GetTextureCount(aiTextureType_DIFFUSE) == 0 &&
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    {
        target.setDiffuseColor(QVector3D(color.r, color.g, color.b));
    }

    if(mat->GetTextureCount(aiTextureType_AMBIENT) == 0 &&
        mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
    {
        target.setAmbientColor(QVector3D(color.r, color.g, color.b));
    }

    // Set floats
    float value = 0;
    if(mat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS)
    {
        target.setShininess(value);
    }

    if(mat->Get(AI_MATKEY_SHININESS_STRENGTH, value) == AI_SUCCESS)
    {
        target.setSpecularIntensity(value);
    }

    else if(mat->GetTextureCount(aiTextureType_SPECULAR) == 0 &&
        mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
    {
        // TODO: We support only grayscale specular color as intensity
        target.setSpecularIntensity(color.r);
    }
}

}