#include "texture2D.h"

#include <QDebug>

#include <gli/gli.hpp>

using namespace Engine;

Texture2D::Texture2D()
    : Texture(false), Resource(), width_(0), height_(0)
{
}

Texture2D::Texture2D(const QString& name, bool loadSrgb)
    : Texture(loadSrgb), Resource(name), width_(0), height_(0)
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

    else if(width < 1 || height < 1)
    {
        return false;
    }

    releaseData();
    gl->glGenTextures(1, &textureId_);

    if(!bind())
    {
        return false;
    }

    gl->glTexImage2D(Target, 0, internalFormat, width, height, 0, format, type, pixels);

    width_ = width;
    height_ = height;

    // Set cached texture flags
    setParameters();

    return true;
}

bool Texture2D::initialiseData(const DataType& data)
{
    gl->glGenTextures(1, &textureId_);
    gl->glBindTexture(Target, textureId_);

    if(gli::is_compressed(data->format()))
    {
        const gli::texture2D& texture = *data;
        gli::internalFormat form = gli::internal_format(texture.format());

        gl->glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, texture.levels() - 1);
        gl->glTexStorage2D(Target, texture.levels(),
            gli::internal_format(texture.format()),
            texture.dimensions().x,
            texture.dimensions().y);

        // Upload mipmaps
        for(gli::texture2D::size_type level = 0; level < texture.levels(); ++level)
        {
            gl->glCompressedTexSubImage2D(Target,
                level, 0, 0,
                texture[level].dimensions().x,
                texture[level].dimensions().y,
                gli::internal_format(texture.format()),
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
        gl->glTexImage2D(Target, 0,
            gli::internal_format(data->format()),
            data->dimensions().x,
            data->dimensions().y, 0,
            gli::external_format(data->format()),
            gli::type_format(data->format()),
            data->data());
    }

    width_ = data->dimensions().x;
    height_ = data->dimensions().y;

    // Set cached texture flags
    setParameters();
    return true;
}

void Texture2D::releaseData()
{
    remove();
    width_ = height_ = 0;
}

GLsizei Texture2D::width() const
{
    return width_;
}

GLsizei Texture2D::height() const
{
    return height_;
}

ResourceData* Texture2D::createData()
{
    TextureData* data = new TextureData(despatcher());
    data->loadSrgb(isSrgb());

    return data;
}

//
// TextureData definitions
//

TextureData::TextureData(ResourceDespatcher* despatcher)
    : ResourceData(despatcher), data_(nullptr), loadSrgb_(false)
{
}

TextureData::~TextureData()
{
    if(data_ != nullptr)
        delete data_;
}

bool TextureData::load(const QString& fileName)
{
    data_ = loadTexture(fileName, loadSrgb_);
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

void TextureData::loadSrgb(bool srgb)
{
    loadSrgb_ = srgb;
}