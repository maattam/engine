//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "textureloader.h"

#include <QImage>
#include <QImageReader>
#include <QFile>
#include <QDebug>

#include <gli/gli.hpp>

using namespace Engine;

namespace {
    bool isDDS(const QString& fileName);
    gli::texture2D* loadDDS(const QString& fileName, TextureConversion conversion);
    gli::texture2D* loadQImage(const QString& fileName, TextureConversion conversion);
}

gli::texture2D* Engine::loadTexture(const QString& fileName, TextureConversion conversion)
{
    gli::texture2D* texture = nullptr;

    // Note: We only support DXT5 for now
    if(isDDS(fileName))
    {
        texture = loadDDS(fileName, conversion);
    }

    // QImage is used to support regular uncompressed image formats.
    // The data is copied into a gli::texture2D container to simplify uploading
    else
    {
        texture = loadQImage(fileName, conversion);
    }

    return texture;
}

namespace {

bool isDDS(const QString& fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
    {
        return false;
    }

    // Read magic header
    return std::strncmp(file.read(3), "DDS", 3) == 0;
}

gli::texture2D* loadDDS(const QString& fileName, TextureConversion conversion)
{
    const gli::storage storage = gli::load_dds(fileName.toUtf8());
    gli::format format = storage.format();

    if(conversion == TC_SRGBA)
    {
        format = gli::format::SRGB_ALPHA_DXT5;
    }

    gli::texture2D* texture = new gli::texture2D(storage, format, 0, 0, 0, 0, 0, storage.levels() - 1);
    if(texture->empty())
    {
        delete texture;
        texture = nullptr;
    }

    return texture;
}

gli::texture2D* loadQImage(const QString& fileName, TextureConversion conversion)
{
    QImageReader reader(fileName);
    QImage image = reader.read();

    if(image.isNull())
    {
        qDebug() << "Failed to read texture:" << fileName << reader.errorString();
        return nullptr;
    }

    // Not very efficient, but make sure the input is in expected byte order.
    QImage argbData = image.convertToFormat(QImage::Format_ARGB32);
    if(argbData.isNull())
    {
        return nullptr;
    }

    gli::format format;
    switch(conversion)
    {
    case TC_GRAYSCALE:
        {
            format = gli::format::R8_UNORM;
            break;
        }

    case TC_SRGBA:
        {
            format = gli::format::SRGB8_ALPHA8;
            break;
        }

    case TC_RGBA:
        {
            format = gli::format::RGBA8_UNORM;
            break;
        }
    }

    gli::texture2D* texture = new gli::texture2D(1, format,
        gli::texture2D::dimensions_type(argbData.width(), argbData.height()));

    
    // Copy bytes from QImage to gli::texture2D
    uchar* linearAddress = texture->data<uchar>();

    // QImage doesn't convert from 32bit to 8bit, so we must copy every fourth byte.
    // Only the red channel is copied over, so validity of the data is not checked.
    if(conversion == TC_GRAYSCALE)
    {
        Q_ASSERT(argbData.byteCount() / 4 == texture->size());

        for(size_t i = 0; i < texture->size(); ++i)
        {
            linearAddress[i] = *(argbData.constBits() + i * 4);
        }
    }

    else
    {
        std::memcpy(linearAddress, argbData.bits(), argbData.byteCount());
    }

    return texture;
}

}