// Base class for different OpenGL texture types

#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"

#include <QList>
#include <QPair>

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

    // Queues a texture parameter, calls glTexParameteri
    virtual void texParameteri(GLenum pname, GLint target);

    // Sets the mag and min filtering to use
    void setFiltering(GLenum magFilter, GLenum minFilter);

    // Sets texture wrapping
    void setWrap(GLenum wrap_s, GLenum wrap_t);

    // Generates mipmaps when the texture is loaded
    virtual void setMipmap(bool enable);
    bool mipmap() const;

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
    // Sets the cached texture parameters and mipmaps
    void setParameters();
    void setDimensions(GLsizei width, GLsizei height);

    GLuint textureId_;

private:
    typedef QPair<GLenum, GLint> Parameteri;
    QList<Parameteri> parametersi_;

    bool mipmap_;
    GLsizei width_;
    GLsizei height_;

    Texture(const Texture&);
    Texture& operator=(const Texture&);
};

#include "texture.inl"

}

#endif // TEXTURE_H