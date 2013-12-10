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

bool Material::bind()
{
    // Bind all textures
    for(int i = 0; i < TEXTURE_COUNT; ++i)
    {
        const Texture2D::Ptr& tex = getTexture(static_cast<TextureType>(i));

        if(tex != nullptr && !tex->bindActive(GL_TEXTURE0 + i))
        {
            return false;
        }
    }

    return true;
}

const Texture2D::Ptr& Material::getTexture(TextureType type)
{
    auto iter = textures_.find(type);
    if(iter == textures_.end())
    {
        qWarning() << __FUNCTION__ << "Material has no texture of type" << type;
        Texture2D::Ptr& tex = textures_[type];

        // Get default texture
        if(type == TEXTURE_SPECULAR || type == TEXTURE_MASK)
        {
            tex = despatcher_->get<Texture2D>(RESOURCE_PATH("images/mask.png"), TC_GRAYSCALE);
        }

        else if(type == TEXTURE_DIFFUSE)
        {
            tex = despatcher_->get<Texture2D>(RESOURCE_PATH("images/white.png"), TC_SRGBA);
        }

        return tex;
    }

    return iter->second;
}

bool Material::hasTexture(TextureType type) const
{
    auto iter = textures_.find(type);
    return iter != textures_.end() && iter->second != nullptr;
}

void Material::setTexture(TextureType type, Texture2D::Ptr& texture)
{
    Q_ASSERT(texture != nullptr);

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

const Material::Attributes& Material::attributes() const
{
    return attributes_;
}

void Material::setAttributes(const Attributes& attrib)
{
    attributes_ = attrib;
}

void Material::setTextureOptions(const Texture2D::Ptr& texture) const
{
    texture->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    texture->setWrap(GL_REPEAT, GL_REPEAT);
    texture->generateMipmaps();

    // Set 16x anisotropy... TODO!
    texture->texParameteri(GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
}