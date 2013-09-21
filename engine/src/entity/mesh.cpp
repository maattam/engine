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

Mesh::Mesh() : Entity(), Resource()
{
}

Mesh::Mesh(const QString& name)
    : Entity(), Resource(name)
{
}

Mesh::~Mesh()
{
}

void Mesh::updateRenderList(RenderList& list)
{
    if(!ready())
        return;

    for(Renderable::SubMesh::Ptr& mesh : entries_)
        list.push_back(mesh.get());
}

bool Mesh::loadData(const QString& fileName)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(fileName.toStdString(),
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);

    if(scene == nullptr)
    {
        qDebug() << "Error loading '" << fileName << "': " << importer.GetErrorString();
        return false;
    }

    return initFromScene(scene, fileName);
}

bool Mesh::initializeData()
{
    entries_.clear();

    entries_.resize(meshData_.size());
    for(size_t i = 0; i < meshData_.size(); ++i)
    {
        MeshData* mesh = meshData_[i].get();

        entries_[i] = std::make_shared<Renderable::SubMesh>();
        entries_[i]->setMaterial(materials_[mesh->materialIndex]);

        if(!entries_[i]->initMesh(mesh->vertices, mesh->normals,
                mesh->tangents, mesh->uvs, mesh->indices))
        {
            return false;
        }
    }

    meshData_.clear();
    materials_.clear();

    setMaterialAttributes(materialAttrib_);

    return true;
}

bool Mesh::initFromScene(const aiScene* scene, const QString& fileName)
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
        meshData_[i] = std::make_shared<MeshData>();
        initSubMesh(mesh, meshData_[i].get());

        meshData_[i]->materialIndex = scene->mMeshes[i]->mMaterialIndex;
        assert(meshData_[i]->materialIndex < materials_.size());
    }

    return true;
}

void Mesh::initSubMesh(const aiMesh* mesh, Mesh::MeshData* data)
{
    const aiVector3D zero3D(0, 0, 0);
    AABB aabb;

    // Fill vertex attribute vectors
    for(size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& pos       = mesh->mVertices[i];
        const aiVector3D& normal    = mesh->mNormals[i];
        const aiVector3D& uv        = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero3D;

        data->vertices.push_back(QVector3D(pos.x, pos.y, pos.z));
        data->normals.push_back(QVector3D(normal.x, normal.y, normal.z));

        if(mesh->HasTangentsAndBitangents())
        {
            const aiVector3D& tangent   = mesh->mTangents[i];
            data->tangents.push_back(QVector3D(tangent.x, tangent.y, tangent.z));
        }

        data->uvs.push_back(QVector2D(uv.x, uv.y));

        // Form bounding rect
        aabb.resize(data->vertices.back());
    }

    updateAABB(aabb);

    // Fill the index buffer
    for(size_t i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);

        data->indices.push_back(face.mIndices[0]);
        data->indices.push_back(face.mIndices[1]);
        data->indices.push_back(face.mIndices[2]);
    }
}

void Mesh::initMaterials(const aiScene* scene, const QString& fileName)
{
    // Extract the directory part of the file name
    int index = fileName.lastIndexOf("/");
    QString dir = ".";

    if(index == 0)
        dir = "/";

    else
        dir = fileName.mid(0, index);

    for(size_t i = 0; i < scene->mNumMaterials; ++i)
    {
        const aiMaterial* material = scene->mMaterials[i];
        aiString path;
        QString fullpath = dir + "/";

        materials_[i] = std::make_shared<Material>(despatcher());

        if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            if(material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                Texture::Ptr texture = despatcher()->get<Texture>(fullpath + path.data);

                if(texture != nullptr)
                {
                    materials_[i]->setTexture(Material::TEXTURE_DIFFUSE, texture);
                }
            }
        }

        if(material->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            if(material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
            {
                Texture::Ptr texture = despatcher()->get<Texture>(fullpath + path.data);

                if(texture != nullptr)
                {
                    materials_[i]->setTexture(Material::TEXTURE_NORMALS, texture);
                }
            }
        }

        if(material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            if(material->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
            {
                Texture::Ptr texture = despatcher()->get<Texture>(fullpath + path.data);

                if(texture != nullptr)
                {
                    materials_[i]->setTexture(Material::TEXTURE_SPECULAR, texture);
                }
            }
        }
    }
}

void Mesh::setMaterialAttributes(const Material::Attributes& attributes)
{
    for(Renderable::SubMesh::Ptr& mesh : entries_)
    {
        if(mesh != nullptr)
        {
            mesh->material()->setAmbientColor(attributes.ambientColor);
            mesh->material()->setSpecularIntensity(attributes.specularIntensity);
            mesh->material()->setDiffuseColor(attributes.diffuseColor);
            mesh->material()->setShininess(attributes.shininess);
        }
    }

    materialAttrib_ = attributes;
}