#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "texture.h"

namespace Engine {

class Texture2D : public Texture<GL_TEXTURE_2D>
{
public:
    Texture2D();
    virtual ~Texture2D();

    // Allocates a texture using glTexImage2D
    virtual bool create(GLint level, GLint internalFormat, GLsizei width, GLsizei height,
        GLint border, GLenum format, GLenum type, const GLvoid* data = nullptr);

    // Allows specifying the mipmap count. Mipmaps have to be created manually using glTexSubImage2D.
    // The texture is left bound if this call succeeds.
    virtual bool createTexStorage(GLint levels, GLint internalFormat, GLsizei width, GLsizei height);
};

}

#endif //TEXTURE2D_H