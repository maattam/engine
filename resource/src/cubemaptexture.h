#ifndef CUBEMAPTEXTURE_H
#define CUBEMAPTEXTURE_H

#include "texture.h"
#include "resource.h"

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

class CubemapTexture
    : public Texture<GL_TEXTURE_CUBE_MAP>, public Resource<CubemapTexture, CubemapData>
{
public:
    CubemapTexture();
    explicit CubemapTexture(const QString& name, TextureConversion conversion = TC_RGBA);

    virtual bool bind();

protected:
    virtual ResourceData* createData();
    virtual bool initialiseData(const DataType& data);
    virtual void releaseData();

    virtual void queryFilesDebug(QStringList& files) const;

private:
    TextureConversion conversion_;
};

}

#endif //CUBEMAPTEXTURE_H