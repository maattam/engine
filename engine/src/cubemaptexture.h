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
    ~CubemapTexture();

    bool create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
        GLenum type, const GLvoid* pixels = nullptr);

protected:
    // fileName has to be in format /path/to/file*.png where file0..5
    // corresponds to cubemap faces
    bool loadData(const QString& fileName);

    bool initializeData();

private:
    QImage* texData_[NUM_TARGETS];
};

}

#endif //CUBEMAPTEXTURE_H