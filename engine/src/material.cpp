#include "material.h"
#include "common.h"
#include "resource/resourcedespatcher.h"

#include <QDebug>

using namespace Engine;

Material::Material(ResourceDespatcher* despatcher)
    : despatcher_(despatcher)
{
}

Material::~Material()
{
}

bool Material::bind()
{
    // Bind all textures
    for(int i = 0; i < TEXTURE_COUNT; ++i)
    {
        const Texture::Ptr& tex = getTexture(static_cast<TextureType>(i));

        if(tex != nullptr && !tex->bind(GL_TEXTURE0 + i))
        {
            return false;
        }
    }

    return true;
}

const Texture::Ptr& Material::getTexture(TextureType type)
{
    auto iter = textures_.find(type);

    if(iter == textures_.end())
    {
        qWarning() << __FUNCTION__ << "Material has no texture of type" << type;
        Texture::Ptr& tex = textures_[type];

        if(type != TEXTURE_NORMALS)
        {
            tex = despatcher_->get<Texture>(RESOURCE_PATH("images/white.png"));
            setTextureOptions(tex);
        }

        return tex;
    }

    return iter->second;
}

bool Material::hasNormals() const
{
    auto iter = textures_.find(TEXTURE_NORMALS);
    return iter != textures_.end() && iter->second != nullptr;
}

void Material::setTexture(TextureType type, Texture::Ptr& texture)
{
    textures_[type] = texture;
    setTextureOptions(texture);
}

void Material::setShininess(float shininess)
{
    attributes_.shininess = shininess;
}

void Material::setAmbientColor(const QVector3D& color)
{
    attributes_.ambientColor = color;
}

void Material::setSpecularIntensity(float intensity)
{
    attributes_.specularIntensity = intensity;
}

void Material::setDiffuseColor(const QVector3D& color)
{
    attributes_.diffuseColor = color;
}

const Material::Attributes& Material::getAttributes() const
{
    return attributes_;
}

void Material::setTextureOptions(const Texture::Ptr& texture) const
{
    if(texture != nullptr)
    {
        texture->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
        texture->setWrap(GL_REPEAT, GL_REPEAT);
        texture->generateMipmaps();

        // Set 16x anisotropy... TODO!
        texture->setAnisotropy(8);
    }
}