//
//  Author   : Matti Määttä
//  Summary  : ShadowMap allows sharing information between shadow rendering methods
//             and lightning shaders.
//

#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "common.h"

#include <QMatrix4x4>
#include <QSize>

namespace Engine {

class ShadowMap
{
public:
    virtual ~ShadowMap() {}

    // Binds all textures starting from location.
    virtual bool bindTextures(GLenum location) = 0;

    virtual void setSize(const QSize& size) = 0;
    virtual const QSize& size() = 0;

    virtual bool create() = 0;

    virtual const QMatrix4x4& lightVP() const = 0;
};

}

#endif // SHADOWMAP_H