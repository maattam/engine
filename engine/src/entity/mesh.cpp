#include "mesh.h"
#include "common.h"
#include "resourcedespatcher.h"
#include  "material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cassert>
#include <QDebug>

using namespace Engine;
using namespace Engine::Entity;

namespace {
    void initMaterialAttributes(aiMaterial* aiMat, const Material::Ptr& target);
}

Mesh::Mesh() : Entity(), Resource()
{
}

Mesh::Mesh(const QString& name) : Entity(), Resource(name)
{
}

Mesh::~Mesh()
{
}

void Mesh::updateRenderList(RenderList& list)
{
    if(!ready())
        return;

    for(SubEntity::Ptr& mesh : entries_)
        mesh->updateRenderList(list);
}

bool Mesh::initialiseData(const DataType& data)
{
    entries_.resize(data.numSubMeshes());
    AABB base;

    for(size_t i = 0; i < data.numSubMeshes(); ++i)
    {
        const MeshData::SubMeshData& mesh = data.subMesh(i);
        Renderable::SubMesh::Ptr subMesh = std::make_shared<Renderable::SubMesh>();

        if(!subMesh->initMesh(mesh.vertices, mesh.normals,
                mesh.tangents, mesh.uvs, mesh.indices))
        {
            return false;
        }

        entries_[i] = std::make_shared<SubEntity>(subMesh,
            data.material(mesh.materialIndex), mesh.aabb);

        base.resize(mesh.aabb);
    }

    updateAABB(base);
    return true;
}

void Mesh::releaseData()
{
    entries_.clear();
}

void Mesh::addSubEntity(const SubEntity::Ptr& subEntity)
{
    entries_.push_back(subEntity);

    AABB aabb = boundingBox();
    if(aabb.resize(subEntity->boundingBox()))
    {
        updateAABB(aabb);
    }
}

Mesh::SubEntityVec::size_type Mesh::numSubEntities() const
{
    return entries_.size();
}

const SubEntity::Ptr& Mesh::subEntity(SubEntityVec::size_type index)
{
    return entries_.at(index);
}

//
// MeshData definitions
//

MeshData::MeshData(ResourceDespatcher* despatcher)
    : ResourceData(despatcher)
{
}

bool MeshData::load(const QString& fileName)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(fileName.toStdString(),
        aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);

    if(scene == nullptr)
    {
        qWarning() << __FUNCTION__ << "Error loading '" << fileName << "': " << importer.GetErrorString();
        return false;
    }

    return initFromScene(scene, fileName);
}

bool MeshData::initFromScene(const aiScene* scene, const QString& fileName)
{
    // Resize to fit
    meshData_.resize(scene->mNumMeshes);
    materials_.resize(scene->mNumMaterials);

    size_t numVertices = 0;
    size_t numIndices = 0;

    // Load materials
    initMaterials(scene, fileName);

    for(size_t i = 0; i < scene->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        initSubMesh(mesh, meshData_[i]);

        meshData_[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
        assert(meshData_[i].materialIndex < materials_.size());
    }

    return true;
}

void MeshData::initSubMesh(const aiMesh* mesh, MeshData::SubMeshData& data)
{
    const aiVector3D zero3D(0, 0, 0);

    // Fill vertex attribute vectors
    for(size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& pos       = mesh->mVertices[i];
        const aiVector3D& normal    = mesh->mNormals[i];
        const aiVector3D& uv        = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero3D;

        QVector3D vertice(pos.x, pos.y, pos.z);
        data.vertices.push_back(vertice);
        data.normals.push_back(QVector3D(normal.x, normal.y, normal.z));

        if(mesh->HasTangentsAndBitangents())
        {
            const aiVector3D& tangent   = mesh->mTangents[i];
            data.tangents.push_back(QVector3D(tangent.x, tangent.y, tangent.z));
        }

        data.uvs.push_back(QVector2D(uv.x, uv.y));

        // Form bounding rect
        data.aabb.resize(vertice);
    }

    // Fill the index buffer
    for(size_t i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);

        data.indices.push_back(face.mIndices[0]);
        data.indices.push_back(face.mIndices[1]);
        data.indices.push_back(face.mIndices[2]);
    }
}

void MeshData::initMaterials(const aiScene* scene, const QString& fileName)
{
    // Create mapping between Material::TextureType and aiTextureType
    const aiTextureType textureMapping[Material::TEXTURE_COUNT] = {
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_SPECULAR
    };

    // Extract the directory part of the file name
    // We assume the material files reside inside the same folder as the model or in its subfolders.
    int index = fileName.lastIndexOf("/");
    QString dir = ".";

    if(index == 0)
        dir = "/";

    else
        dir = fileName.mid(0, index);

    for(size_t i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* material = scene->mMaterials[i];
        aiString path;
        QString fullpath = dir + "/";

        materials_[i] = std::make_shared<Material>(despatcher());

        // Query supported materials
        for(int j = 0; j < Material::TEXTURE_COUNT; ++j)
        {
            if(material->GetTextureCount(textureMapping[j]) > 0 &&
                material->GetTexture(textureMapping[j], 0, &path) == aiReturn_SUCCESS)
            {
                const Material::TextureType type = static_cast<Material::TextureType>(j);
                Texture2D::Ptr texture = despatcher()->get<Texture2D>(fullpath + path.data,
                    type == Material::TEXTURE_DIFFUSE);

                materials_[i]->setTexture(type, texture);
            }
        }

        // Hack: Workaround for broken waveform normals
        if(material->GetTextureCount(aiTextureType_HEIGHT) > 0 &&
            material->GetTexture(aiTextureType_HEIGHT, 0, &path) == aiReturn_SUCCESS)
        {
            Texture2D::Ptr texture = despatcher()->get<Texture2D>(fullpath + path.data);
            materials_[i]->setTexture(Material::TEXTURE_NORMALS, texture);
        }

        initMaterialAttributes(material, materials_[i]);
    }
}

MeshData::SubMeshVec::size_type MeshData::numSubMeshes() const
{
    return meshData_.size();
}

const MeshData::SubMeshData& MeshData::subMesh(SubMeshVec::size_type index) const
{
    return meshData_.at(index);
}

MeshData::MaterialVec::size_type MeshData::numMaterials() const
{
    return materials_.size();
}

const Material::Ptr& MeshData::material(MaterialVec::size_type index) const
{
    return materials_.at(index);
}

MeshData::MeshData(ResourceDespatcher* despatcher,
                   const SubMeshVec& subMeshes, const MaterialVec& materials)
                   : ResourceData(despatcher), meshData_(subMeshes), materials_(materials)
{
}

namespace {
    void initMaterialAttributes(aiMaterial* mat, const Material::Ptr& target)
    {
        if(mat == nullptr)
            return;

        aiColor3D color;

        // If color map texture was not set, check if attribute exists instead
        if(mat->GetTextureCount(aiTextureType_DIFFUSE) == 0 &&
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
        {
            target->setDiffuseColor(QVector3D(color.r, color.g, color.b));
        }

        if(mat->GetTextureCount(aiTextureType_AMBIENT) == 0 &&
            mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
        {
            target->setAmbientColor(QVector3D(color.r, color.g, color.b));
        }

        // Set floats
        float value = 0;
        if(mat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS)
        {
            target->setShininess(value);
        }

        if(mat->Get(AI_MATKEY_SHININESS_STRENGTH, value) == AI_SUCCESS)
        {
            target->setSpecularIntensity(value);
        }

        else if(mat->GetTextureCount(aiTextureType_SPECULAR) == 0 &&
            mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
        {
            // TODO: We support only grayscale specular color as intensity
            target->setSpecularIntensity(color.r);
        }
    }
}