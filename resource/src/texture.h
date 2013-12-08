// Base class for different OpenGL texture types

#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"

#include <vector>

namespace gli {
    class texture2D;
}

namespace Engine {

template<GLenum Type>
class Texture
{
public:
    enum { Target = Type };

    // If loadSrgb is true, the texture is mapped to linear color space in shaders
    explicit Texture(bool loadSrgb);
    virtual ~Texture() = 0;

    // Deletes the texture using glDeleteTextures.
    void remove();

    // Queues a texture parameter, calls glTexParameteri
    void texParameteri(GLenum pname, GLint target);

    // Sets the mag and min filtering to use
    void setFiltering(GLenum magFilter, GLenum minFilter);

    // Sets texture wrapping
    void setWrap(GLenum wrap_s, GLenum wrap_t);

    // Generated mipmaps when the texture is loaded
    void generateMipmaps();

    // Attempts to bind the texture, returns false on failure
    virtual bool bind();

    // Attemps to bind the texture to the given texture slot
    bool bindActive(GLenum target);

    GLuint textureId() const;

    // Tells whether the texture is using SRGB texture format.
    bool isSrgb() const;

protected:
    void setParameters();

    GLuint textureId_;
    bool mipmaps_;
    bool srgb_;

private:
    typedef std::pair<GLenum, GLint> Parameteri;
    std::vector<Parameteri> parametersi_;
};

// Allocates a new texture, returns nullptr on failure,
// If srgb is true, the texture is converted to linear color space upon loading.
gli::texture2D* loadTexture(const QString& fileName, bool srgb = false);

#include "texture.inl"

}

#endif // TEXTURE_H