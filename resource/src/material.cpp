//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "material.h"

#include "common.h"
#include "textureloader.h"
#include "texture2dresource.h"
#include "binder.h"

#include <QDebug>

using namespace Engine;

namespace {
    Material::TexturePtr makeDefault(const QString& fileName, TextureConversion conversion);

    Material::TexturePtr fetchDefault(Material::TextureType type, const QString& fileName, TextureConversion conversion);

    // Returns the default 1x1 texture for the type.
    Material::TexturePtr defaultTexture(Material::TextureType type);

    // Static default texture watchers.
    std::array<std::weak_ptr<Texture2D>, Material::TEXTURE_COUNT> nullTextures;
}

Material::Material()
    : renderType_(RENDER_OPAQUE)
{
}

Material::~Material()
{
}

Material::Attributes::Attributes()
{
    // Some sane default values
    ambientColor = QVector3D(0, 0, 0);
    diffuseColor = QVector3D(1, 1, 1);
    shininess = 25.0f;
    specularIntensity = 0.2f;
}

bool Material::bind()
{
    // Bind all textures
    for(int i = 0; i < TEXTURE_COUNT; ++i)
    {
        const TexturePtr& tex = getTexture(static_cast<TextureType>(i));

        if(tex != nullptr && !Binder::bind(tex, GL_TEXTURE0 + i))
        {
            return false;
        }
    }

    return true;
}

const Material::TexturePtr& Material::getTexture(TextureType type)
{
    TexturePtr& tex = textures_[type];
    if(!tex && type != TEXTURE_NORMALS)
    {
        // qWarning() << __FUNCTION__ << "Material has no texture of type" << type;
        tex = defaultTexture(type);
    }

    return tex;
}

bool Material::hasTexture(TextureType type) const
{
    return textures_[type] != nullptr;
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

    if(color != QVector3D(0, 0, 0))
    {
        renderType_ = RENDER_EMISSIVE;
    }
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

    if(attrib.ambientColor != QVector3D(0, 0, 0))
    {
        renderType_ = RENDER_EMISSIVE;
    }
}

void Material::setTextureOptions(const TexturePtr& texture) const
{
    texture->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    texture->setWrap(GL_REPEAT, GL_REPEAT);
    texture->generateMipmap();

    // Set 16x anisotropy... TODO!
    texture->texParameteri(GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
}

void Material::setRenderType(RenderType type)
{
    renderType_ = type;
}

Material::RenderType Material::renderType() const
{
    return renderType_;
}

namespace {
    Material::TexturePtr defaultTexture(Material::TextureType type)
    {
        Material::TexturePtr target;

        if(type == Material::TEXTURE_DIFFUSE)
        {
            target = fetchDefault(type, RESOURCE_PATH("images/white.png"), TC_RGBA);
        }

        else if(type == Material::TEXTURE_MASK)
        {
            target = fetchDefault(type, RESOURCE_PATH("images/mask.png"), TC_GRAYSCALE);
        }

        else if(type == Material::TEXTURE_SPECULAR)
        {
            target = fetchDefault(type, RESOURCE_PATH("images/spec.png"), TC_GRAYSCALE);
        }

        else if(type == Material::TEXTURE_SHININESS)
        {
            target = fetchDefault(type, RESOURCE_PATH("images/spec.png"), TC_GRAYSCALE);
        }

        return target;
    }

    Material::TexturePtr makeDefault(const QString& fileName, TextureConversion conversion)
    {
        std::shared_ptr<Texture2DResource> resource(new Texture2DResource(fileName, conversion));
        if(resource->load(fileName))
        {
            return resource;
        }

        return nullptr;
    }

    Material::TexturePtr fetchDefault(Material::TextureType type, const QString& fileName, TextureConversion conversion)
    {
        Material::TexturePtr target;

        std::weak_ptr<Texture2D>& handle = nullTextures[type];
        if(handle.expired())
        {
            target = makeDefault(fileName, conversion);
            handle = target;
        }

        else
        {
            target = handle.lock();
        }

        return target;
    }
}