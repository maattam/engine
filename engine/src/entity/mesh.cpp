#include "mesh.h"
#include "common.h"
#include "resourcedespatcher.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cassert>
#include <QDebug>

using namespace Engine;
using namespace Engine::Entity;

Mesh::Mesh() : Entity()
{
}

Mesh::~Mesh()
{
}

void Mesh::updateRenderList(RenderList& list)
{
    for(Renderable::SubMesh::Ptr& mesh : entries_)
        list.push_back(mesh.get());
}

bool Mesh::load(const QString& fileName)
{
    entries_.clear();

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

void Mesh::setMaterialAttributes(const Material::Attributes& attributes)
{
    for(Material::Ptr& mat : materials_)
    {
        if(mat != nullptr)
        {
            mat->setAmbientColor(attributes.ambientColor);
            mat->setSpecularIntensity(attributes.specularIntensity);
            mat->setDiffuseColor(attributes.diffuseColor);
        }
    }
}

bool Mesh::initFromScene(const aiScene* scene, const QString& fileName)
{
    // Resize to fit
    entries_.resize(scene->mNumMeshes);
    materials_.resize(scene->mNumMaterials);

    size_t numVertices = 0;
    size_t numIndices = 0;

    // Load materials
    initMaterials(scene, fileName);

    for(size_t i = 0; i < scene->mNumMeshes; ++i)
    {
        std::vector<QVector3D> vertices, normals, tangents;
        std::vector<QVector2D> uvs;
        std::vector<unsigned int> indices;

        const aiMesh* mesh = scene->mMeshes[i];
        initSubMesh(mesh, vertices, normals, tangents, uvs, indices);

        unsigned int materialIndex = scene->mMeshes[i]->mMaterialIndex;
        assert(materialIndex < materials_.size());

        entries_[i] = std::make_shared<Renderable::SubMesh>(gl);
        entries_[i]->setMaterial(materials_[materialIndex]);
        entries_[i]->initMesh(vertices, normals, tangents, uvs, indices);
    }

    return true;
}

void Mesh::initSubMesh(const aiMesh* mesh,
              std::vector<QVector3D>& vertices,
              std::vector<QVector3D>& normals,
              std::vector<QVector3D>& tangents,
              std::vector<QVector2D>& uvs,
              std::vector<unsigned int>& indices)
{
    const aiVector3D zero3D(0, 0, 0);

    // Fill vertex attribute vectors
    for(size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& pos       = mesh->mVertices[i];
        const aiVector3D& normal    = mesh->mNormals[i];
        const aiVector3D& uv        = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero3D;

        vertices.push_back(QVector3D(pos.x, pos.y, pos.z));
        normals.push_back(QVector3D(normal.x, normal.y, normal.z));

        if(mesh->HasTangentsAndBitangents())
        {
            const aiVector3D& tangent   = mesh->mTangents[i];
            tangents.push_back(QVector3D(tangent.x, tangent.y, tangent.z));
        }

        uvs.push_back(QVector2D(uv.x, uv.y));
    }

    // Fill the index buffer
    for(size_t i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);

        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
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