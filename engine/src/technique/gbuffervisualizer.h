//
//  Author   : Matti Määttä
//  Summary  : Allows direct debugging of GBuffer's textures.
//

#ifndef GBUFFERVISUALIZER_H
#define GBUFFERVISUALIZER_H

#include "dsmaterialshader.h"
#include "../gbuffer.h"

namespace Engine { namespace Technique {

class GBufferVisualizer : public DSMaterialShader
{
public:
    GBufferVisualizer();
    virtual ~GBufferVisualizer();

    enum TextureType { GB_POSITIONS, GB_NORMALS, GB_DIFFUSE, GB_COUNT };

    // Sets the used output channel
    // precondition: Technique is enabled
    void outputTexture(TextureType type);

protected:
    virtual bool init();
};

}}

#endif // GBUFFERVISUALIZER_H