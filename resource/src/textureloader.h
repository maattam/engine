//
//  Author   : Matti Määttä
//  Summary  : Loads texture data from disk.
//

#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <QString>

namespace gli {
    class texture2D;
}

namespace Engine {

// Allocates a new texture, returns nullptr on failure,
// The TextureConversion field affects the created texture's internal format:
// TC_RGBA is the regular RGBA texture
// TC_SRGBA is a regular RGBA texture mapped to linear color space
// TC_GRAYSCALE is a grayscale image (all components are the same, alpha is 0)
enum TextureConversion { TC_RGBA, TC_SRGBA, TC_GRAYSCALE };
gli::texture2D* loadTexture(const QString& fileName, TextureConversion conversion = TC_RGBA);

}

#endif // TEXTURELOADER_H