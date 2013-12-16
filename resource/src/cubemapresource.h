#ifndef CUBEMAPRESOURCE_H
#define CUBEMAPRESOURCE_H

#include "cubemaptexture.h"
#include "resource.h"
#include "textureloader.h"

namespace Engine {

class CubemapData : public ResourceData
{
public:
    enum { Faces = 6 };

    explicit CubemapData(ResourceDespatcher* despatcher);
    ~CubemapData();

    // precondition: fileName has to be in format /path/to/file*.png where file0..5
    //               corresponds to cubemap faces
    virtual bool load(const QString& fileName);
    gli::texture2D* at(unsigned int index) const;

    // Sets the texture conversion to use upon loading
    // If not set, the default TC_RGBA is used.
    void setConversion(TextureConversion conversion);

private:
    gli::texture2D* textures_[Faces];
    TextureConversion conversion_;
};

class CubemapResource : public CubemapTexture,
    public Resource<CubemapResource, CubemapData>
{
public:
    CubemapResource();
    explicit CubemapResource(const QString& name, TextureConversion conversion = TC_RGBA);

    // Fails if the resource is managed
    virtual bool create(GLenum face, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
        GLint border, GLenum format, GLenum type, const GLvoid* data = nullptr);

    virtual bool bind();

protected:
    virtual ResourceData* createData();
    virtual bool initialiseData(const DataType& data);
    virtual void releaseData();

    virtual void queryFilesDebug(QStringList& files) const;

private:
    TextureConversion conversion_;

    void uploadCompressed(const DataType& data);
};

}

#endif // CUBEMAPRESOURCE_H