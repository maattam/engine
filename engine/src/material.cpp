#include "material.h"

#include <QDebug>

using namespace Engine;

Material::Material(QOPENGL_FUNCTIONS* funcs) : gl(funcs)
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
        qDebug() << "Warning: Material has no texture of type '" << type << "'";

        Texture::Ptr& tex = textures_[type];
        if(!loadNullTexture(tex))
        {
            qDebug() << "Failed to load null texture";
        }

        return tex;
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

bool Material::loadNullTexture(Texture::Ptr& texture) const
{
    // Load 1x1 white texture
    texture = std::make_shared<Texture>(gl);

    if(!texture->loadFromFile(":/images/white.png"))
    {
        return false;
    }

    setTextureOptions(texture);
    return true;
}

void Material::setTextureOptions(const Texture::Ptr& texture) const
{
    if(texture != nullptr)
    {
        texture->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
        texture->setWrap(GL_REPEAT, GL_REPEAT);
        texture->generateMipmaps();

        // Force 16x anisotropy.. TODO!
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
    }
}