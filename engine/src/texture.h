#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "resource.h"
#include <memory>

namespace Engine {

class Texture : public Resource
{
public:
    typedef std::shared_ptr<Texture> Ptr;

    Texture();
    ~Texture();

    void create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
        GLenum type, const GLvoid* pixels = nullptr);

    bool load(const QString& fileName);

    void setFiltering(GLenum magFilter, GLenum minFilter);
    void setWrap(GLenum wrap_s, GLenum wrap_t);
    void generateMipmaps();

    bool bind();
    bool bind(GLenum target);

    GLuint textureId();

    bool bound() const;

private:
    GLuint textureId_;
    bool bound_;

    static GLuint boundTextureId_;
};

}

#endif //TEXTURE_H