#include "dsmaterialshader.h"

#include "common.h"
#include "gbuffer.h"

using namespace Engine;
using namespace Engine::Technique;

DSMaterialShader::DSMaterialShader()
    : Technique(), gbuffer_(nullptr), samples_(1), depthRange_(0, 1)
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

void DSMaterialShader::setDepthRange(float rnear, float rfar)
{
    depthRange_.setX(rnear);
    depthRange_.setY(rfar);

    if(program()->isLinked())
    {
        setUniformValue("depthRange", depthRange_);
    }
}

void DSMaterialShader::setProjMatrix(const QMatrix4x4& proj)
{
    setUniformValue("persProj", proj);
}

void DSMaterialShader::setLightDirection(const QVector3D& dir)
{
    setUniformValue("lightDirection", dir.normalized());
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

    if(!setUniformValue("depthRange", depthRange_))
    {
        return false;
    }

    if(!useSubroutine("nullLight", GL_FRAGMENT_SHADER))
    {
        return false;
    }

    return resolveUniformLocation("persProj") != -1;
}