//
//  Author   : Matti Määttä
//  Summary  : 
//

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
        setUniformValue("inputTexture", textureUnit_);
    }
}

void BlurFilter::setTextureParams(int width, int height, float lodLevel)
{
    setUniformValue("width", width);
    setUniformValue("height", height);
    setUniformValue("lodLevel", lodLevel);
}

bool BlurFilter::init()
{
    if(!setUniformValue("inputTexture", textureUnit_))
    {
        return false;
    }

    resolveUniformLocation("width");
    resolveUniformLocation("height");
    resolveUniformLocation("lodLevel");
    return true;
}