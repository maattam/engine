#include "blurfilter.h"

using namespace Engine;
using namespace Engine::Technique;

BlurFilter::BlurFilter()
    : Technique(), textureUnit_(-1)
{
}

BlurFilter::~BlurFilter()
{
}

void BlurFilter::setTextureUnit(int unit)
{
    textureUnit_ = unit;

    if(program()->isLinked())
    {
        program()->setUniformValue(cachedUniformLocation("inputTexture"), textureUnit_);
    }
}

void BlurFilter::setTextureParams(int width, int height, float lodLevel)
{
    program()->setUniformValue(cachedUniformLocation("width"), width);
    program()->setUniformValue(cachedUniformLocation("height"), height);
    program()->setUniformValue(cachedUniformLocation("lodLevel"), lodLevel);
}

bool BlurFilter::init()
{
    int unitLocation = resolveUniformLocation("inputTexture");
    program()->setUniformValue(unitLocation, textureUnit_);

    resolveUniformLocation("width");
    resolveUniformLocation("height");
    resolveUniformLocation("lodLevel");
    return true;
}