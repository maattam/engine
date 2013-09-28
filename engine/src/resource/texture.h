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

    Texture();
    virtual ~Texture();

    void remove();
    void texParameteri(GLenum pname, GLint target);

    void setFiltering(GLenum magFilter, GLenum minFilter);
    void setWrap(GLenum wrap_s, GLenum wrap_t);
    void generateMipmaps();

    virtual bool bind();
    bool bindActive(GLenum target);

    GLuint textureId() const;

protected:
    void setParameters();

    GLuint textureId_;
    bool mipmaps_;

private:
    typedef std::pair<GLenum, GLint> Parameteri;
    std::vector<Parameteri> parametersi_;
};

// Allocates a new texture, returns nullptr on failure
gli::texture2D* loadTexture(const QString& fileName);

#include "texture.inl"

}

#endif // TEXTURE_H