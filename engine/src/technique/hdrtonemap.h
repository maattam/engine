// A post-processing shader which handles tonemapping, gamma correction and bloom.

#ifndef HDRTONEMAP_H
#define HDRTONEMAP_H

#include "technique.h"

namespace Engine { namespace Technique {

class HDRTonemap : public Technique
{
public:
    HDRTonemap(unsigned int samples, unsigned int bloomLod);
    virtual ~HDRTonemap();

    virtual bool enable();

    // Adds a new shader to the program
    // precondition: shader not null
    // postcondition: shader will be linked when enable is called
    virtual void addShader(const Shader::Ptr& shader);

    void setInputTexture(int textureId);
    void setBloomTexture(int textureId);

    // Sets shader attributes
    void setBloomFactor(float factor);
    void setExposure(float exposure);
    void setGamma(float gamma);
    void setBrightLevel(float bright);

protected:
    virtual bool init();

private:
    int inputTextureId_;
    int bloomTextureId_;

    float bloomFactor_;
    float exposure_;
    float gamma_;
    float bright_;

    unsigned int samples_;
    unsigned int bloomLod_;

    bool attributeChanged_;
};

}}

#endif // HDRTONEMAP_H