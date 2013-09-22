#include "cubemaptexture.h"

#include <QDebug>
#include <QImage>

using namespace Engine;

const GLenum TYPES[CubemapTexture::NUM_TARGETS] = {   GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                                      GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                                      GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                                                      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                                      GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                                                      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

CubemapTexture::CubemapTexture() : Texture()
{
    setTarget(GL_TEXTURE_CUBE_MAP);
}

CubemapTexture::CubemapTexture(const QString& name) : Texture(name)
{
    setTarget(GL_TEXTURE_CUBE_MAP);
}

CubemapTexture::~CubemapTexture()
{
}

bool CubemapTexture::create(GLsizei width, GLsizei height, GLint internalFormat, GLint format,
        GLenum type, const GLvoid* pixels)
{
    qWarning() << __FUNCTION__ << "Unimplemented";
    return false;
}

bool CubemapTexture::loadData(const QString& fileName)
{
    for(int i = 0; i < NUM_TARGETS; ++i)
    {
        QString file = fileName;
        QImage image;

        if(!image.load(file.replace(QString("*"), QString::number(i))))
        {
            qWarning() << __FUNCTION__ << "Failed to load" << file;
            return false;
        }
 
        qDebug() << __FUNCTION__ << "Loaded" << file;
        texData_[i] = new QImage(image.convertToFormat(QImage::Format_ARGB32));
    }

    return true;
}

bool CubemapTexture::initializeData()
{
    gl->glGenTextures(1, &textureId_);
    gl->glBindTexture(GL_TEXTURE_CUBE_MAP, textureId_);

    for(int i = 0; i < NUM_TARGETS; ++i)
    {
        gl->glTexImage2D(TYPES[i], 0, GL_RGBA, texData_[i]->width(), texData_[i]->height(),
            0, GL_BGRA, GL_UNSIGNED_BYTE, texData_[i]->bits());

        delete texData_[i];
        texData_[i] = nullptr;
    }

    setFlags();

    gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}