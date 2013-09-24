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
    virtual ~Texture();

    // Fails if the resource is managed
    virtual bool create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
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
    virtual bool loadData(const QString& fileName);
    virtual bool initializeData();
    virtual void releaseData();

    // Change texture target from default GL_TEXTURE_2D
    void setTarget(GLenum target);
    void setFlags();

    GLuint textureId_;

private:
    bool mipmapping_;

    QImage* texData_;
    std::vector<std::pair<GLenum, GLenum>> texFlags_;

    GLenum target_;
};

}

#endif //TEXTURE_H