#include "dsmaterialshader.h"

#include "../gbuffer.h"

using namespace Engine::Technique;

DSMaterialShader::DSMaterialShader()
    : Technique(), samples_(1)
{
}

DSMaterialShader::~DSMaterialShader()
{
}

void DSMaterialShader::setSampleCount(unsigned int count)
{
    samples_ = count;

    if(program()->isLinked())
    {
        program()->setUniformValue(samplesLocation_, samples_);
    }
}

void DSMaterialShader::setDepthRange(float rnear, float rfar)
{
    program()->setUniformValue(depthRangeLocation_, QVector2D(rnear, rfar));
}

void DSMaterialShader::setInvProjMatrix(const QMatrix4x4& invProj)
{
    program()->setUniformValue(invProjLocation_, invProj);
}

void DSMaterialShader::setViewport(const QRect& viewport)
{
    viewport_ = QVector4D(viewport.left(), viewport.top(), viewport.width(), viewport.height());

    if(program()->isLinked())
    {
        program()->setUniformValue(viewportLocation_, viewport_);
    }
}

void DSMaterialShader::init()
{
    const char* SAMPLERS[GBuffer::TEXTURE_COUNT] = {
        "normalSpecData", "diffuseSpecData", "depthData",
    };

    // Set texture samplers
    for(int i = 0; i < GBuffer::TEXTURE_COUNT; ++i)
    {
        program()->setUniformValue(SAMPLERS[i], i);
    }

    samplesLocation_ = program()->uniformLocation("samples");
    program()->setUniformValue(samplesLocation_, samples_);

    viewportLocation_ = program()->uniformLocation("viewport");
    program()->setUniformValue(viewportLocation_, viewport_);

    depthRangeLocation_ = program()->uniformLocation("depthRange");
    invProjLocation_ = program()->uniformLocation("inverseProj");
}