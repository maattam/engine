#include "dsmaterialshader.h"

#include "common.h"
#include "gbuffer.h"

using namespace Engine;
using namespace Engine::Technique;

DSMaterialShader::DSMaterialShader()
    : Technique(), gbuffer_(nullptr), samples_(1)
{
}

DSMaterialShader::~DSMaterialShader()
{
}

void DSMaterialShader::setGBuffer(GBuffer const* gbuffer)
{
    gbuffer_ = gbuffer;
}

void DSMaterialShader::setSampleCount(unsigned int count)
{
    samples_ = count;

    if(program()->isLinked())
    {
        setUniformValue("samples", samples_);
    }
}

void DSMaterialShader::setProjMatrix(const QMatrix4x4& proj)
{
    setUniformValue("invPersProj", proj.inverted());
}

bool DSMaterialShader::init()
{
    if(gbuffer_ == nullptr)
    {
        return false;
    }

    // Set texture samplers
    int i = 0;
    for(const QString& texture : gbuffer_->textures())
    {
        if(!setUniformValue(texture + "Data", i++))
        {
            return false;
        }
    }

    // The other uniforms are not critical as they can be replaced by subclasses
    if(!setUniformValue("samples", samples_))
    {
        return false;
    }

    return resolveUniformLocation("invPersProj") != -1;
}