#include "material.h"
#include "common.h"
#include "resourcedespatcher.h"

#include <QDebug>

using namespace Engine;

Material::Material(ResourceDespatcher* despatcher)
    : despatcher_(despatcher)
{
}

Material::~Material()
{
}

const Texture::Ptr& Material::getTexture(TextureType type)
{
    auto iter = textures_.find(type);

    // If texture was not set, return null texture
    if(iter == textures_.end() || iter->second == nullptr)
    {
        qWarning() << __FUNCTION__ << "Material has no texture of type '" << type << "'";
        textures_[type] = despatcher_->get<Texture>(":/images/white.png");

        return textures_[type];
    }

    return iter->second;
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
        texture->setAnisotropy(16);
    }
}