#include "hdrtonemap.h"

using namespace Engine;
using namespace Engine::Technique;

HDRTonemap::HDRTonemap()
    : Technique(), inputTextureId_(-1), inputSamples_(1),
    bloomFactor_(1.0f), exposure_(1.0f), gamma_(2.2f), bright_(1.0f)
{
}

HDRTonemap::~HDRTonemap()
{
}

void HDRTonemap::setInputTexture(int textureId, int samples)
{
    inputTextureId_ = textureId;
    inputSamples_ = samples;

    if(program()->isLinked())
    {
        setUniformValue("inputTexture", inputTextureId_);
        setUniformValue("samples", inputSamples_);
    }
}

void HDRTonemap::setBloomTexture(int textureId, int bloomLevels)
{
    bloomTextureId_ = textureId;
    bloomLevels_ = bloomLevels;

    if(program()->isLinked())
    {
        setUniformValue("bloomTexture", bloomTextureId_);
        setUniformValue("bloomLevels", bloomLevels_);
    }
}

void HDRTonemap::setBloomFactor(float factor)
{
    bloomFactor_ = factor;
    if(program()->isLinked())
    {
        setUniformValue("bloomFactor", factor);
    }
}

void HDRTonemap::setExposure(float exposure)
{
    exposure_ = exposure;
    if(program()->isLinked())
    {
        setUniformValue("exposure", exposure);
    }
}

void HDRTonemap::setGamma(float gamma)
{
    gamma_ = gamma;
    if(program()->isLinked())
    {
        setUniformValue("gamma", gamma);
    }
}

void HDRTonemap::setBrightLevel(float bright)
{
    bright_ = bright;
    if(program()->isLinked())
    {
        setUniformValue("bright", bright);
    }
}

bool HDRTonemap::init()
{
    if(!setUniformValue("inputTexture", inputTextureId_))
    {
        return false;
    }

    if(!setUniformValue("samples", inputSamples_))
    {
        return false;
    }

    if(!setUniformValue("bloomTexture", bloomTextureId_))
    {
        return false;
    }

    if(!setUniformValue("bloomLevels", bloomLevels_))
    {
        return false;
    }

    // Cba to verify..
    setUniformValue("bloomFactor", bloomFactor_);
    setUniformValue("exposure", exposure_);
    setUniformValue("gamma", gamma_);
    setUniformValue("bright", bright_);

    return true;
}