#include "texture.h"

#include <QImage>
#include <QFile>

#include <gli/gli.hpp>

namespace {
    bool isDDS(const QString& fileName);
}

gli::texture2D* Engine::loadTexture(const QString& fileName, bool srgb)
{
    gli::texture2D* texture = nullptr;
    QImage image;

    // Note: We only support DXT5
    if(isDDS(fileName))
    {
        const gli::storage storage = gli::load_dds(fileName.toUtf8());
        gli::format format = storage.format();

        if(srgb)
        {
            format = gli::format::SRGB_ALPHA_DXT5;
        }

        texture = new gli::texture2D(storage, format, 0, 0, 0, 0, 0, storage.levels() - 1);
        if(texture->empty())
        {
            delete texture;
            texture = nullptr;
        }
    }

    // QImage is used to support regular uncompressed image formats.
    // The data is copied to a gli::texture2D container to simplify uploading
    else if(image.load(fileName))
    {
        QImage argbData = image.convertToFormat(QImage::Format_ARGB32);
        gli::format format = gli::format::RGBA8_UNORM;

        if(srgb)
        {
            format = gli::format::SRGB8_ALPHA8;
        }

        texture = new gli::texture2D(1, format,
            gli::texture2D::dimensions_type(argbData.width(), argbData.height()));

        uchar* linearAddress = texture->data<uchar>();
        std::memcpy(linearAddress, argbData.bits(), argbData.byteCount());
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

}