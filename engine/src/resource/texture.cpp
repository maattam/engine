#include "texture.h"

#include <QImage>
#include <QDebug>
#include <QFile>

#include <gli/gli.hpp>

using namespace Engine;

Texture::Texture()
    : Resource(), textureId_(0), mipmapping_(false), textureData_(nullptr)
{
    target_ = GL_TEXTURE_2D;
}

Texture::Texture(const QString& name)
    : Resource(name), textureId_(0), mipmapping_(false), textureData_(nullptr)
{
    target_ = GL_TEXTURE_2D;
}

Texture::~Texture()
{
    releaseData();

    if(textureData_ != nullptr)
        delete textureData_;
}

void Texture::releaseData()
{
    if(textureId_ != 0)
    {
        gl->glDeleteTextures(1, &textureId_);
    }
}

GLuint Texture::textureId()
{
    return textureId_;
}

bool Texture::create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
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
    gl->glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, pixels);

    return true;
}

bool Texture::loadData(const QString& fileName)
{
    if(textureData_ != nullptr)
        delete textureData_;

    QImage image;

    if(isDDS(fileName))
    {
        textureData_ = new gli::texture2D(gli::loadStorageDDS(fileName.toStdString()));
        if(textureData_->empty())
        {
            delete textureData_;
            textureData_ = nullptr;

            return false;
        }
    }

    // QImage is used to support regular uncompressed image formats.
    // The data is copied to a gli::texture2D container to simplify uploading
    else if(image.load(fileName))
    {
        QImage argbData = image.convertToFormat(QImage::Format_ARGB32);

        textureData_ = new gli::texture2D(1, gli::RGBA8U,
            gli::texture2D::dimensions_type(argbData.width(), argbData.height()));

        uchar* linearAddress = textureData_->data<uchar>();
        std::memcpy(linearAddress, argbData.bits(), argbData.byteCount());
    }

    else
    {
        return false;
    }

    return true;
}

bool Texture::initializeData()
{
    assert(textureData_ != nullptr || !textureData_->empty());

    // Delete old texture
    if(textureId_ != 0)
    {
        gl->glDeleteTextures(1, &textureId_);
    }

    gl->glGenTextures(1, &textureId_);
    gl->glBindTexture(GL_TEXTURE_2D, textureId_);

    if(gli::is_compressed(textureData_->format()))
    {
        const gli::texture2D& texture = *textureData_;

        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, texture.levels() - 1);
        gl->glTexStorage2D(GL_TEXTURE_2D, texture.levels(),
            gli::internal_format(texture.format()),
            texture.dimensions().x,
            texture.dimensions().y);

        // Upload mipmaps
        for(gli::texture2D::size_type level = 0; level < texture.levels(); ++level)
        {
            gl->glCompressedTexSubImage2D(GL_TEXTURE_2D,
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
            mipmapping_ = false;
        }
    }

    else
    {
        // QImage format, hack
        gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureData_->dimensions().x,
                textureData_->dimensions().y, 0, GL_BGRA, GL_UNSIGNED_BYTE, textureData_->data());
    }

    // Clean up the data used to uploading the texture
    delete textureData_;
    textureData_ = nullptr;

    // Set cached texture flags
    setFlags();

    return true;
}

void Texture::setFiltering(GLenum magFilter, GLenum minFilter)
{
    if(bind())
    {
        gl->glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, magFilter);
        gl->glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, minFilter);
    }

    else
    {
        texFlags_.push_back(std::make_pair(GL_TEXTURE_MAG_FILTER, magFilter));
        texFlags_.push_back(std::make_pair(GL_TEXTURE_MIN_FILTER, minFilter));
    }
}

void Texture::setWrap(GLenum wrap_s, GLenum wrap_t)
{
    if(bind())
    {
        gl->glTexParameteri(target_, GL_TEXTURE_WRAP_S, wrap_s);
        gl->glTexParameteri(target_, GL_TEXTURE_WRAP_T, wrap_t);
    }

    else
    {
        texFlags_.push_back(std::make_pair(GL_TEXTURE_WRAP_S, wrap_s));
        texFlags_.push_back(std::make_pair(GL_TEXTURE_WRAP_T, wrap_t));
    }
}

void Texture::setAnisotropy(GLint samples)
{
    if(bind())
    {
        gl->glTexParameteri(target_, GL_TEXTURE_MAX_ANISOTROPY_EXT, samples);
    }

    else
    {
        texFlags_.push_back(std::make_pair(GL_TEXTURE_MAX_ANISOTROPY_EXT, samples));
    }
}

void Texture::generateMipmaps()
{
    if(bind())
    {
        gl->glGenerateMipmap(target_);
    }

    mipmapping_ = true;
}

bool Texture::bind()
{
    if(!ready() || textureId_ == 0)
        return false;

    gl->glBindTexture(target_, textureId_);

    return true;
}

bool Texture::bind(GLenum target)
{
    gl->glActiveTexture(target);
    return bind();
}

void Texture::setTarget(GLenum target)
{
    target_ = target;
}

void Texture::setFlags()
{
    // Set cached texture flags
    for(auto it = texFlags_.begin(); it != texFlags_.end(); ++it)
    {
        gl->glTexParameteri(target_, it->first, it->second);
    }

    texFlags_.clear();

    if(mipmapping_)
    {
        gl->glGenerateMipmap(target_);
    }
}

bool Texture::isDDS(const QString& fileName) const
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
    {
        return false;
    }

    // Read magic header
    return std::strncmp(file.read(3), "DDS", 3) == 0;
}