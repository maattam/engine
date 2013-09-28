#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "texture.h"
#include "resource.h"

#include <memory>
#include <vector>

namespace gli {
    class texture2D;
}

namespace Engine {

class TextureData : public ResourceData
{
public:
    TextureData(ResourceDespatcher* despatcher);
    ~TextureData();

    virtual bool load(const QString& fileName);

    gli::texture2D* operator->() const;
    gli::texture2D& operator*() const;

private:
    gli::texture2D* data_;
};

class Texture2D : public Texture<GL_TEXTURE_2D>, public Resource<Texture2D, TextureData>
{
public:
    Texture2D();
    Texture2D(const QString& name);

    // Fails if the resource is managed
    bool create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
        GLenum type, const GLvoid* pixels = nullptr);

    virtual bool bind();

protected:
    virtual bool initialiseData(DataType& data);
    virtual void releaseData();
};

}

#endif //TEXTURE2D_H