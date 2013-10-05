#include "texture2D.h"

#include <QDebug>

#include <gli/gli.hpp>

using namespace Engine;

Texture2D::Texture2D() : Texture(), Resource(), srgb_(false)
{
}

Texture2D::Texture2D(const QString& name) : Texture(), Resource(name), srgb_(false)
{
}

bool Texture2D::bind()
{
    return ready() && Texture::bind();
}

bool Texture2D::create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
                     GLenum type, const GLvoid* pixels)
{
    if(managed())
    {
        qWarning() << __FUNCTION__ << "Can't modify managed resource";
        return false;
    }

    releaseData();

    gl->glGenTextures(1, &textureId_);
    bind();
    gl->glTexImage2D(Target, 0, internalFormat, width, height, 0, format, type, pixels);

    return true;
}

bool Texture2D::initialiseData(const DataType& data)
{
    gl->glGenTextures(1, &textureId_);
    gl->glBindTexture(Target, textureId_);

    if(gli::is_compressed(data->format()))
    {
        const gli::texture2D& texture = *data;

        gl->glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, texture.levels() - 1);
        gl->glTexStorage2D(Target, texture.levels(),
            //gli::internal_format(texture.format()),
            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
            texture.dimensions().x,
            texture.dimensions().y);
        
        // Upload mipmaps
        for(gli::texture2D::size_type level = 0; level < texture.levels(); ++level)
        {
            gl->glCompressedTexSubImage2D(Target,
                level, 0, 0,
                texture[level].dimensions().x,
                texture[level].dimensions().y,
                //gli::internal_format(texture.format()),
                GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
                texture[level].size(),
                texture[level].data());
        }

        // Don't generate mipmaps again
        if(texture.levels() > 1)
        {
            mipmaps_ = false;
        }
    }

    else
    {
        GLint internalFormat = GL_RGBA;

        if(srgb_)
        {
            internalFormat = gli::internal_format(data->format());
        }

        // QImage format, hack
        gl->glTexImage2D(Target, 0, internalFormat, data->dimensions().x,
                data->dimensions().y, 0, GL_BGRA, GL_UNSIGNED_BYTE, data->data());
    }

    // Set cached texture flags
    setParameters();
    return true;
}

void Texture2D::releaseData()
{
    remove();
}

void Texture2D::setSRGB(bool srgb)
{
    srgb_ = srgb;
}

//
// TextureData definitions
//

TextureData::TextureData(ResourceDespatcher* despatcher)
    : ResourceData(despatcher), data_(nullptr)
{
}

TextureData::~TextureData()
{
    if(data_ != nullptr)
        delete data_;
}

bool TextureData::load(const QString& fileName)
{
    data_ = loadTexture(fileName);
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