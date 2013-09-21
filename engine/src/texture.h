#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "resource.h"

#include <memory>
#include <vector>

namespace Engine {

class Texture : public Resource
{
public:
    typedef std::shared_ptr<Texture> Ptr;

    Texture();
    Texture(const QString& name);
    ~Texture();

    // Fails if the resource is managed
    bool create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
        GLenum type, const GLvoid* pixels = nullptr);

    void setFiltering(GLenum magFilter, GLenum minFilter);
    void setAnisotropy(GLint samples);
    void setWrap(GLenum wrap_s, GLenum wrap_t);
    void generateMipmaps();

    bool bind();
    bool bind(GLenum target);

    GLuint textureId();

    bool bound() const;

protected:
    bool loadData(const QString& fileName);
    bool initializeData();

private:
    GLuint textureId_;
    bool mipmapping_;

    QImage* texData_;
    std::vector<std::pair<GLenum, GLenum>> texFlags_;
};

}

#endif //TEXTURE_H