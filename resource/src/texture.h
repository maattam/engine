// Base class for different OpenGL texture types

#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"

namespace Engine {

template<GLenum Type>
class Texture
{
public:
    enum { Target = Type };

    Texture();
    virtual ~Texture() = 0;

    // Deletes the texture using glDeleteTextures.
    // postcondition: textureId == 0
    void remove();

    // Calls glTexParameteri
    // precondition: texture is bound.
    virtual void texParameteri(GLenum pname, GLint target);

    // Sets the mag and min filtering to use
    // precondition: texture is bound.
    void setFiltering(GLenum magFilter, GLenum minFilter);

    // Sets texture wrapping
    // precondition: texture is bound.
    void setWrap(GLenum wrap_s, GLenum wrap_t);

    // Generates mipmaps when the texture is bound.
    // Needs to be called again when the texture changes.
    // precondition: texture is bound.
    virtual void generateMipmap();

    // Attempts to bind the texture, returns false on failure
    virtual bool bind();

    // Attemps to bind the texture to the given texture slot
    bool bindActive(GLenum target);

    // Returns the texture handle. 0 if texture is no texture is generated.
    GLuint textureId() const;

    // Returns texture dimensions
    GLsizei width() const;
    GLsizei height() const;

protected:
    void setDimensions(GLsizei width, GLsizei height);

    GLuint textureId_;

private:
    GLsizei width_;
    GLsizei height_;

    Texture(const Texture&);
    Texture& operator=(const Texture&);
};

#include "texture.inl"

}

#endif // TEXTURE_H