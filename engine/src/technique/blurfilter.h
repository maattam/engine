//
//  Author   : Matti M‰‰tt‰
//  Summary  : Applies gaussian blur to the input texture.
//

#ifndef BLURFILTER_H
#define BLURFILTER_H

#include "technique.h"

namespace Engine { namespace Technique {

class BlurFilter : public Technique
{
public:
    BlurFilter();
    virtual ~BlurFilter();

    // Sets the input texture unit
    void setTextureUnit(int unit);

    // Sets the texture size and the used lod level. Size must correspond to the given lod level.
    // Precondition: Technique is enabled
    void setTextureParams(int width, int height, float lodLevel);

protected:
    virtual bool init();

private:
    int textureUnit_;
};

}}

#endif // BLURFILTER_H