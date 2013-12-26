#include "texture2d.h"

using namespace Engine;

Texture2D::Texture2D()
    : Texture()
{
}

Texture2D::~Texture2D()
{
}

bool Texture2D::create(GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                       GLint border, GLenum format, GLenum type, const GLvoid* data)
{
    if(width < 1 || height < 1)
    {
        return false;
    }

    remove();

    gl->glGenTextures(1, &textureId_);
    gl->glBindTexture(Target, textureId_);
    gl->glTexImage2D(Target, level, internalFormat, width, height, border, format, type, data);

    setDimensions(width, height);
    return gl->glGetError() == GL_NO_ERROR;
}

bool Texture2D::createTexStorage(GLint levels, GLint internalFormat, GLsizei width, GLsizei height)
{
    if(width < 1 || height < 1)
    {
        return false;
    }

    remove();

    gl->glGenTextures(1, &textureId_);
    gl->glBindTexture(Target, textureId_);
    gl->glTexStorage2D(Target, levels, internalFormat, width, height);

    setDimensions(width, height);
    return gl->glGetError() == GL_NO_ERROR;
}