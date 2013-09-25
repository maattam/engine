#include "texture.h"

#include <QImage>
#include <QDebug>

using namespace Engine;

Texture::Texture()
    : Resource(), textureId_(0), mipmapping_(false), texData_(nullptr)
{
    target_ = GL_TEXTURE_2D;
}

Texture::Texture(const QString& name)
    : Resource(name), textureId_(0), mipmapping_(false), texData_(nullptr)
{
    target_ = GL_TEXTURE_2D;
}

Texture::~Texture()
{
    releaseData();

    if(texData_ != nullptr)
        delete texData_;
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
    QImage image;

    if(!image.load(fileName))
    {
        return false;
    }

    texData_ = new QImage(image.convertToFormat(QImage::Format_ARGB32));
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