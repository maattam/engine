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

    // Sets the texture conversion to use upon loading
    // If not set, the default TC_RGBA is used.
    void setConversion(TextureConversion conversion);

private:
    gli::texture2D* data_;
    TextureConversion conversion_;
};

class Texture2D : public Texture<GL_TEXTURE_2D>, public Resource<Texture2D, TextureData>
{
public:
    Texture2D();
    Texture2D(const QString& name, TextureConversion conversion = TC_RGBA);

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
    TextureConversion conversion_;
};

}

#endif //TEXTURE2D_H