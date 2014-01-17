//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "texture2dresource.h"

#include <QDebug>

#include <gli/gli.hpp>

using namespace Engine;

Texture2DResource::Texture2DResource()
    : Texture2D(), Resource(), conversion_(TC_RGBA), mipmap_(false)
{
}

Texture2DResource::Texture2DResource(const QString& name, TextureConversion conversion)
    : Texture2D(), Resource(name), conversion_(conversion), mipmap_(false)
{
}

bool Texture2DResource::bind()
{
    return ready() && Texture2D::bind();
}

void Texture2DResource::texParameteri(GLenum pname, GLint target)
{
    parametersi_.push_back(qMakePair(pname, target));

    if(ready())
    {
        Texture2D::texParameteri(pname, target);
    }
}

void Texture2DResource::generateMipmap()
{
    mipmap_ = true;

    if(ready())
    {
        Texture2D::generateMipmap();
    }
}

bool Texture2DResource::create(GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                       GLint border, GLenum format, GLenum type, const GLvoid* data)
{
    if(managed())
    {
        qWarning() << __FUNCTION__ << "Can't modify managed resource";
        return false;
    }

    return Texture2D::create(level, internalFormat, width, height, border, format, type, data);
}

bool Texture2DResource::createTexStorage(GLint levels, GLint internalFormat, GLsizei width, GLsizei height)
{
    if(managed())
    {
        qWarning() << __FUNCTION__ << "Can't modify managed resource";
        return false;
    }

    return Texture2D::createTexStorage(levels, internalFormat, width, height);
}

bool Texture2DResource::initialiseData(const DataType& data)
{
    const gli::texture2D& texture = *data;

    // If we are dealing with a non-compressed texture, just call the default initialiser.
    if(!gli::is_compressed(texture.format()))
    {
        if(!Texture2D::create(0,
            gli::internal_format(data->format()),
            data->dimensions().x,
            data->dimensions().y, 0,
            gli::external_format(data->format()),
            gli::type_format(data->format()),
            data->data()))
        {
            return false;
        }
    }

    else if(!uploadCompressed(texture))
    {
        return false;
    }

    // Set cached parameters
    for(const auto& pair : parametersi_)
    {
        Texture2D::texParameteri(pair.first, pair.second);
    }

    if(mipmap_)
    {
        Texture2D::generateMipmap();
    }

    return true;
}

bool Texture2DResource::uploadCompressed(const gli::texture2D& texture)
{
    // If the texture has pre-baked mipmaps, upload them using glTexStorage.
    // NOTE: glTexStorage seems to ignore srgb flag using AMD drivers 13.x
    if(texture.levels() > 1)
    {
        mipmap_ = false;

        if(!Texture2D::createTexStorage(static_cast<GLint>(texture.levels()),
            gli::internal_format(texture.format()),
            static_cast<GLsizei>(texture.dimensions().x),
            static_cast<GLsizei>(texture.dimensions().y)))
        {
            return false;
        }

        gl->glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, texture.levels() - 1);

        // Upload mipmaps
        for(gli::texture2D::size_type level = 0; level < texture.levels(); ++level)
        {
            gl->glCompressedTexSubImage2D(Target,
                level, 0, 0,
                texture[level].dimensions().x,
                texture[level].dimensions().y,
                gli::internal_format(texture.format()),
                static_cast<GLsizei>(texture[level].size()),
                texture[level].data());
        }
    }

    else
    {
        gl->glCompressedTexImage2D(Target, 0,
            gli::internal_format(texture.format()),
            texture.dimensions().x,
            texture.dimensions().y,
            0,
            static_cast<GLsizei>(texture.size()),
            texture.data());
    }

    return gl->glGetError() == GL_NO_ERROR;
}

void Texture2DResource::releaseResource()
{
    remove();
}

Texture2DResource::ResourceDataPtr Texture2DResource::createData()
{
    std::shared_ptr<TextureData> data(new TextureData());
    data->setConversion(conversion_);

    return data;
}

//
// TextureData definitions
//

TextureData::TextureData()
    : data_(nullptr), conversion_(TC_RGBA)
{
}

TextureData::~TextureData()
{
    if(data_ != nullptr)
        delete data_;
}

bool TextureData::load(const QString& fileName)
{
    data_ = loadTexture(fileName, conversion_);
    return data_ != nullptr;
}

gli::texture2D* TextureData::operator->() const
{
    return data_;
}

gli::texture2D& TextureData::operator*() const
{
    return *data_;
}

void TextureData::setConversion(TextureConversion conversion)
{
    conversion_ = conversion;
}