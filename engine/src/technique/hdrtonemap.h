// A post-processing shader which handles tonemapping, gamma correction and bloom.

#ifndef HDRTONEMAP_H
#define HDRTONEMAP_H

#include "technique.h"

namespace Engine { namespace Technique {

class HDRTonemap : public Technique
{
public:
    HDRTonemap();
    virtual ~HDRTonemap();

    virtual bool enable();

    void setInputTexture(int textureId, int samples);
    void setBloomTexture(int textureId, int bloomLevels);

    // Sets shader attributes
    void setBloomFactor(float factor);
    void setExposure(float exposure);
    void setGamma(float gamma);
    void setBrightLevel(float bright);

protected:
    virtual bool init();

private:
    int inputTextureId_;
    int inputSamples_;

    int bloomTextureId_;
    int bloomLevels_;

    float bloomFactor_;
    float exposure_;
    float gamma_;
    float bright_;

    bool attributeChanged_;
};

}}

#endif // HDRTONEMAP_H