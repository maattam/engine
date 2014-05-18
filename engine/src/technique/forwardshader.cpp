#include "forwardshader.h"

#include "mathelp.h"

using namespace Engine;
using namespace Engine::Technique;

ForwardShader::ForwardShader() : Technique()
{
}

ForwardShader::~ForwardShader()
{
}

void ForwardShader::setMVP(const QMatrix4x4& mvp)
{
    setUniformValue("MVP", mvp);
}

void ForwardShader::setMaterialAttributes(const Material& material)
{
    const Material::Attributes& attrib = material.attributes();

    setUniformValue("material.diffuse", linearColor(attrib.diffuseColor));
    setUniformValue("material.ambient", linearColor(attrib.ambientColor));

    if(material.renderType() == Material::RENDER_TRANSPARENT)
    {
        setUniformValue("material.alpha", attrib.alpha);
    }

    else
    {
        setUniformValue("material.alpha", 1.0f);
    }
}

void ForwardShader::setDepthTextureUnit(int unit)
{
    depthUnit_ = unit;
}

bool ForwardShader::init()
{
    if(!Technique::init())
    {
        return false;
    }

    setUniformValue("material.diffuseSampler", static_cast<int>(Material::TEXTURE_DIFFUSE));
    setUniformValue("depth", depthUnit_);

    return true;
}