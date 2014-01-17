//
//  Author   : Matti Määttä
//  Summary  : OpenGL cubemap texture.
//

#ifndef CUBEMAPTEXTURE_H
#define CUBEMAPTEXTURE_H

#include "texture.h"

namespace Engine {

class CubemapTexture : public Texture<GL_TEXTURE_CUBE_MAP>
{
public:
    CubemapTexture();
    virtual ~CubemapTexture();

    // Creates a cubemap face using glTexImage2D.
    // The texture is generated during the first call to this function. If the texture needs to be removed to
    // create a new texture, remove() has to be called before calling this function.
    virtual bool create(GLenum face, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
        GLint border, GLenum format, GLenum type, const GLvoid* data = nullptr);
};

}

#endif //CUBEMAPTEXTURE_H