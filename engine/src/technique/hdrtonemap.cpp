//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "hdrtonemap.h"

using namespace Engine;
using namespace Engine::Technique;

HDRTonemap::HDRTonemap(unsigned int samples, unsigned int bloomLod)
    : Technique(), inputTextureId_(-1), bloomFactor_(1.0f), exposure_(1.0f),
    gamma_(2.2f), bright_(1.0f), attributeChanged_(true), samples_(samples), bloomLod_(bloomLod)
{
}

HDRTonemap::~HDRTonemap()
{
}

bool HDRTonemap::enable()
{
    if(!Technique::enable())
    {
        return false;
    }

    if(attributeChanged_)
    {
        attributeChanged_ = false;

        setUniformValue("bloomFactor", bloomFactor_);
        setUniformValue("exposure", exposure_);
        setUniformValue("gamma", gamma_);
        setUniformValue("bright", bright_);
    }

    return true;
}

void HDRTonemap::addShader(const Shader::Ptr& shader)
{
    Technique::addShader(shader);

    if(shader->type() == Shader::Type::Fragment)
    {
        ShaderData::DefineMap defines;
        defines.insert("SAMPLES", samples_);
        defines.insert("BLOOMLOD", bloomLod_);

        shader->setNamedDefines(defines);
    }
}

void HDRTonemap::setInputTexture(int textureId)
{
    inputTextureId_ = textureId;

    if(program()->isLinked())
    {
        setUniformValue("inputTexture", inputTextureId_);
    }
}

void HDRTonemap::setBloomTexture(int textureId)
{
    bloomTextureId_ = textureId;

    if(program()->isLinked())
    {
        setUniformValue("bloomTexture", bloomTextureId_);
    }
}

void HDRTonemap::setBloomFactor(float factor)
{
    bloomFactor_ = factor;
    attributeChanged_ = true;
}

void HDRTonemap::setExposure(float exposure)
{
    exposure_ = exposure;
    attributeChanged_ = true;
}

void HDRTonemap::setGamma(float gamma)
{
    gamma_ = gamma;
    attributeChanged_ = true;
}

void HDRTonemap::setBrightLevel(float bright)
{
    bright_ = bright;
    attributeChanged_ = true;
}

bool HDRTonemap::init()
{
    if(!setUniformValue("inputTexture", inputTextureId_))
    {
        return false;
    }

    if(!setUniformValue("bloomTexture", bloomTextureId_))
    {
        return false;
    }

    return true;
}