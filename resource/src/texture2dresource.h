#ifndef TEXTURE2DRESOURCE_H
#define TEXTURE2DRESOURCE_H

#include "texture2d.h"
#include "resource.h"
#include "textureloader.h"
#include "resourcedata.h"

#include <QList>
#include <QPair>

namespace Engine {

class TextureData : public ResourceData
{
public:
    explicit TextureData();
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

class Texture2DResource : public Texture2D,
    public Resource<Texture2DResource, TextureData>
{
public:
    Texture2DResource();
    Texture2DResource(const QString& name, TextureConversion conversion = TC_RGBA);

    virtual void texParameteri(GLenum pname, GLint target);
    virtual void generateMipmap();

    // Fails if the resource is managed
    virtual bool create(GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                       GLint border, GLenum format, GLenum type, const GLvoid* data = nullptr);

    // Fails if the resource is managed
    virtual bool createTexStorage(GLint levels, GLint internalFormat, GLsizei width, GLsizei height);

    virtual bool bind();

protected:
    virtual ResourceData* createData();
    virtual bool initialiseData(const DataType& data);
    virtual void releaseData();

private:
    TextureConversion conversion_;

    typedef QPair<GLenum, GLint> Parameteri;
    QList<Parameteri> parametersi_;
    bool mipmap_;

    bool uploadCompressed(const gli::texture2D& texture);
};

}

#endif // TEXTURE2DRESOURCE_H