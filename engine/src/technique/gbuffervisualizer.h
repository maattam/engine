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
    virtual void init();
};

}}

#endif // GBUFFERVISUALIZER_H