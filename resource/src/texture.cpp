#include "texture.h"

#include <QImage>
#include <QFile>

#include <gli/gli.hpp>

namespace {
    bool isDDS(const QString& fileName);
}

gli::texture2D* Engine::loadTexture(const QString& fileName)
{
    gli::texture2D* texture = nullptr;
    QImage image;

    if(isDDS(fileName))
    {
        texture = new gli::texture2D(gli::loadStorageDDS(fileName.toStdString()));

        if(texture->empty())
        {
            return false;
        }
    }

    // QImage is used to support regular uncompressed image formats.
    // The data is copied to a gli::texture2D container to simplify uploading
    else if(image.load(fileName))
    {
        QImage argbData = image.convertToFormat(QImage::Format_ARGB32);

        texture = new gli::texture2D(1, gli::SRGB8_ALPHA8,
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