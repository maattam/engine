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

    explicit Texture(bool loadSrgb);
    virtual ~Texture() = 0;

    // Deletes the texture using glDeleteTextures.
    void remove();
    void texParameteri(GLenum pname, GLint target);

    void setFiltering(GLenum magFilter, GLenum minFilter);
    void setWrap(GLenum wrap_s, GLenum wrap_t);
    void generateMipmaps();

    virtual bool bind();
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