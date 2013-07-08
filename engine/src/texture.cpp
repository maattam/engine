#include "texture.h"

#include <QImage>

using namespace Engine;

GLuint Texture::boundTextureId_ = 0;

Texture::Texture(QOpenGLFunctions_4_2_Core* funcs)
    : gl(funcs), textureId_(0), bound_(false)
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

void Texture::create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
                     GLenum type, const GLvoid* pixels)
{
    // Delete old texture
    if(textureId_ != 0)
    {
        gl->glDeleteTextures(1, &textureId_);
    }

    gl->glGenTextures(1, &textureId_);
    bind();
    gl->glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, pixels);
}

bool Texture::loadFromFile(const std::string& fileName)
{
    // Delete old texture
    if(textureId_ != 0)
    {
        gl->glDeleteTextures(1, &textureId_);
    }

    QImage image;

    if(!image.load(fileName.c_str()))
    {
        return false;
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    int width = image.width();

    gl->glGenTextures(1, &textureId_);
    bind();
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

    return true;
}

void Texture::setFiltering(GLenum magFilter, GLenum minFilter)
{
    if(bind())
    {
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    }
}

void Texture::setWrap(GLenum wrap_s, GLenum wrap_t)
{
    if(bind())
    {
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    }
}

void Texture::generateMipmaps()
{
    if(bind())
    {
        gl->glGenerateMipmap(GL_TEXTURE_2D);
    }
}

bool Texture::bind()
{
    if(textureId_ == 0)
        return false;

    if(!bound())
    {
        glBindTexture(GL_TEXTURE_2D, textureId_);
        boundTextureId_ = textureId_;
    }

    return true;
}

bool Texture::bind(GLenum target)
{
    gl->glActiveTexture(target);
    return bind();
}

bool Texture::bound() const
{
    if(textureId_ != 0 && boundTextureId_ == textureId_)
        return true;

    return false;
}