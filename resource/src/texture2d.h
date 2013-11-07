#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "texture.h"
#include "resource.h"

#include <memory>
#include <vector>

namespace Engine {

class TextureData : public ResourceData
{
public:
    explicit TextureData(ResourceDespatcher* despatcher);
    ~TextureData();

    virtual bool load(const QString& fileName);

    gli::texture2D* operator->() const;
    gli::texture2D& operator*() const;

    void loadSrgb(bool srgb);

private:
    gli::texture2D* data_;
    bool loadSrgb_;
};

class Texture2D : public Texture<GL_TEXTURE_2D>, public Resource<Texture2D, TextureData>
{
public:
    Texture2D();
    Texture2D(const QString& name, bool loadSrgb = false);

    // Fails if the resource is managed
    bool create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
        GLenum type, const GLvoid* pixels = nullptr);

    virtual bool bind();

    GLsizei width() const;
    GLsizei height() const;

protected:
    virtual ResourceData* createData();
    virtual bool initialiseData(const DataType& data);
    virtual void releaseData();

private:
    GLsizei width_;
    GLsizei height_;
};

}

#endif //TEXTURE2D_H