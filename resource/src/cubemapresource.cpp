#include "cubemapresource.h"

#include <QDebug>

#include <gli/gli.hpp>

using namespace Engine;

const GLenum FACES[CubemapData::Faces] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

CubemapResource::CubemapResource()
    : CubemapTexture(), Resource()
{
}

CubemapResource::CubemapResource(const QString& name, TextureConversion conversion)
    : CubemapTexture(), Resource(name), conversion_(conversion)
{
}

void CubemapResource::texParameteri(GLenum pname, GLint target)
{
    parametersi_.push_back(qMakePair(pname, target));

    if(ready())
    {
        CubemapTexture::texParameteri(pname, target);
    }
}

bool CubemapResource::bind()
{
    return ready() && CubemapTexture::bind();
}

bool CubemapResource::create(GLenum face, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                             GLint border, GLenum format, GLenum type, const GLvoid* data)
{
    if(managed())
    {
        qWarning() << __FUNCTION__ << "Can't modify managed resource";
        return false;
    }

    return CubemapTexture::create(face, level, internalFormat, width, height, border, format, type, data);
}

bool CubemapResource::initialiseData(const DataType& data)
{
    if(gli::is_compressed(data.at(0)->format()))
    {
        uploadCompressed(data);
    }

    else
    {
        for(int i = 0; i < CubemapData::Faces; ++i)
        {
            const gli::texture2D* tex = data.at(i);

            if(!CubemapTexture::create(FACES[i], 0,
                gli::internal_format(tex->format()),
                tex->dimensions().x,
                tex->dimensions().y,
                0,
                gli::external_format(tex->format()),
                gli::type_format(tex->format()),
                tex->data()))
            {
                return false;
            }
        }
    }

    gl->glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(Target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Set cached parameters
    for(const auto& pair : parametersi_)
    {
        CubemapTexture::texParameteri(pair.first, pair.second);
    }

    return true;
}

void CubemapResource::uploadCompressed(const DataType& data)
{
    gl->glGenTextures(1, &textureId_);
    gl->glBindTexture(Target, textureId_);

    for(int i = 0; i < CubemapData::Faces; ++i)
    {
        const gli::texture2D* tex = data.at(i);

        // TODO: Mipmaps?
        gl->glCompressedTexImage2D(FACES[i], 0,
            gli::internal_format(tex->format()),
            tex->dimensions().x,
            tex->dimensions().y,
            0,
            static_cast<GLsizei>(tex->size()),
            tex->data());
    }
}

void CubemapResource::releaseData()
{
    remove();
}

void CubemapResource::queryFilesDebug(QStringList& files) const
{
    for(int i = 0; i < CubemapData::Faces; ++i)
    {
        QString fn = name();
        files.append(fn.replace(QString("*"), QString::number(i)));
    }
}

ResourceData* CubemapResource::createData()
{
    CubemapData* data = new CubemapData(despatcher());
    data->setConversion(conversion_);

    return data;
}

//
// CubemapData
//

CubemapData::CubemapData(ResourceDespatcher* despatcher)
    : ResourceData(despatcher), conversion_(TC_RGBA)
{
    for(int i = 0; i < CubemapData::Faces; ++i)
        textures_[i] = nullptr;
}

CubemapData::~CubemapData()
{
    for(gli::texture2D* tex : textures_)
    {
        if(tex != nullptr)
            delete tex;
    }
}

bool CubemapData::load(const QString& fileName)
{
    for(int i = 0; i < CubemapData::Faces; ++i)
    {
        QString file = fileName;
        file.replace(QString("*"), QString::number(i));

        textures_[i] = loadTexture(file, conversion_);
        if(textures_[i] == nullptr)
        {
            return false;
        }
    }

    return true;
}

gli::texture2D* CubemapData::at(unsigned int index) const
{
    return textures_[index];
}

void CubemapData::setConversion(TextureConversion conversion)
{
    conversion_ = conversion;
}