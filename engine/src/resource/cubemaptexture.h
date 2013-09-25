#ifndef CUBEMAPTEXTURE_H
#define CUBEMAPTEXTURE_H

#include "texture.h"

namespace Engine {

class CubemapTexture : public Texture
{
public:
    enum { NUM_TARGETS = 6 };

    CubemapTexture();
    CubemapTexture(const QString& name);

    virtual bool create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
        GLenum type, const GLvoid* pixels = nullptr);

protected:
    // fileName has to be in format /path/to/file*.png where file0..5
    // corresponds to cubemap faces
    virtual bool loadData(const QString& fileName);

    virtual bool initializeData();
    virtual void queryFilesDebug(QStringList& files) const;

private:
    QImage* texData_[NUM_TARGETS];
};

}

#endif //CUBEMAPTEXTURE_H