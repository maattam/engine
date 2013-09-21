#include "texture.h"

#include <QImage>
#include <QDebug>

using namespace Engine;

Texture::Texture()
    : Resource(), textureId_(0), mipmapping_(false), texData_(nullptr)
{
}

Texture::Texture(const QString& name)
    : Resource(name), textureId_(0), mipmapping_(false), texData_(nullptr)
{
}

Texture::~Texture()
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
        qDebug() << "Texture::create(): Can't modify managed resource";
        return false;
    }

    // Delete old texture
    if(textureId_ != 0)
    {
        gl->glDeleteTextures(1, &textureId_);
    }

    gl->glGenTextures(1, &textureId_);
    bind();
    gl->glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, pixels);

    return true;
}

bool Texture::loadData(const QString& fileName)
{
    QImage image;

    if(!image.load(fileName))
    {
        return false;
    }

    image = image.convertToFormat(QImage::Format_ARGB32);
    texData_ = new QImage(image);

    return true;
}

bool Texture::initializeData()
{
    // Delete old texture
    if(textureId_ != 0)
    {
        gl->glDeleteTextures(1, &textureId_);
    }

    gl->glGenTextures(1, &textureId_);
    gl->glBindTexture(GL_TEXTURE_2D, textureId_);

    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texData_->width(), texData_->height(), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, texData_->bits());

    // Clean up the data used to uploading the texture
    delete texData_;
    texData_ = nullptr;

    // Set cached texture flags
    for(auto it = texFlags_.begin(); it != texFlags_.end(); ++it)
    {
        gl->glTexParameteri(GL_TEXTURE_2D, it->first, it->second);
    }

    texFlags_.clear();

    if(mipmapping_)
    {
        gl->glGenerateMipmap(GL_TEXTURE_2D);
    }

    return true;
}

void Texture::setFiltering(GLenum magFilter, GLenum minFilter)
{
    if(bind())
    {
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
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
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
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
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, samples);
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
        gl->glGenerateMipmap(GL_TEXTURE_2D);
    }

    mipmapping_ = true;
}

bool Texture::bind()
{
    if(!ready() || textureId_ == 0)
        return false;

    gl->glBindTexture(GL_TEXTURE_2D, textureId_);

    return true;
}

bool Texture::bind(GLenum target)
{
    gl->glActiveTexture(target);
    return bind();
}