#include "material.h"

#include "common.h"
#include "textureloader.h"
#include "texture2dresource.h"

#include <QDebug>

using namespace Engine;

namespace {
    Material::TexturePtr makeDefault(const QString& fileName, TextureConversion conversion);
}

Material::Material()
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
        const TexturePtr& tex = getTexture(static_cast<TextureType>(i));

        if(tex != nullptr && !tex->bindActive(GL_TEXTURE0 + i))
        {
            return false;
        }
    }

    return true;
}

const Material::TexturePtr& Material::getTexture(TextureType type)
{
    auto iter = textures_.find(type);
    if(iter == textures_.end())
    {
        //qWarning() << __FUNCTION__ << "Material has no texture of type" << type;

        TexturePtr& tex = textures_[type];
        tex = defaultTexture(type);

        return tex;
    }

    return iter->second;
}

bool Material::hasTexture(TextureType type) const
{
    auto iter = textures_.find(type);
    return iter != textures_.end() && iter->second != nullptr;
}

void Material::setTexture(TextureType type, const TexturePtr& texture)
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

void Material::setTextureOptions(const TexturePtr& texture) const
{
    texture->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    texture->setWrap(GL_REPEAT, GL_REPEAT);
    texture->generateMipmap();

    // Set 16x anisotropy... TODO!
    texture->texParameteri(GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
}

Material::TexturePtr Material::defaultTexture(TextureType type)
{
    TexturePtr target;

    if(type == TEXTURE_DIFFUSE)
    {
        if(nullDiffuseTexture.expired())
        {
            target = makeDefault(RESOURCE_PATH("images/white.png"), TC_RGBA);
            nullDiffuseTexture = target;
        }

        else
        {
            target = nullDiffuseTexture.lock();
        }
    }

    else if(type == TEXTURE_MASK)
    {
        if(nullMaskTexture.expired())
        {
            target = makeDefault(RESOURCE_PATH("images/mask.png"), TC_GRAYSCALE);
            nullMaskTexture = target;
        }

        else
        {
            target = nullMaskTexture.lock();
        }
    }

    else if(type == TEXTURE_SPECULAR)
    {
        if(nullSpecularTexture.expired())
        {
            target = makeDefault(RESOURCE_PATH("images/mask.png"), TC_GRAYSCALE);
            nullSpecularTexture = target;
        }

        else
        {
            target = nullSpecularTexture.lock();
        }
    }

    return target;
}

std::weak_ptr<Texture2D> Material::nullMaskTexture;
std::weak_ptr<Texture2D> Material::nullDiffuseTexture;
std::weak_ptr<Texture2D> Material::nullSpecularTexture;

namespace {
    Material::TexturePtr makeDefault(const QString& fileName, TextureConversion conversion)
    {
        std::shared_ptr<Texture2DResource> resource(new Texture2DResource(fileName, conversion));
        if(resource->load(fileName))
        {
            return resource;
        }

        return nullptr;
    }
}