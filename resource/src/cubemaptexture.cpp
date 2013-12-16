#include "cubemaptexture.h"

using namespace Engine;

CubemapTexture::CubemapTexture()
    : Texture()
{
}

CubemapTexture::~CubemapTexture()
{
}

bool CubemapTexture::create(GLenum face, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                            GLint border, GLenum format, GLenum type, const GLvoid* data)
{
    if(width < 1 || height < 1)
    {
        return false;
    }

    if(textureId_ == 0)
    {
        gl->glGenTextures(1, &textureId_);
    }

    gl->glBindTexture(Target, textureId_);
    gl->glTexImage2D(face, level, internalFormat, width, height, border, format, type, data);

    setDimensions(width, height);
    setParameters();

    return true;
}