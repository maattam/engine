#include "dsmaterialshader.h"

#include "../gbuffer.h"

using namespace Engine::Technique;

DSMaterialShader::DSMaterialShader()
    : Technique(), samples_(1), depthRange_(0, 1)
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
        program()->setUniformValue(cachedUniformLocation("samples"), samples_);
    }
}

void DSMaterialShader::setDepthRange(float rnear, float rfar)
{
    depthRange_.setX(rnear);
    depthRange_.setY(rfar);

    if(program()->isLinked())
    {
        program()->setUniformValue(cachedUniformLocation("depthRange"), depthRange_);
    }
}

void DSMaterialShader::setProjMatrix(const QMatrix4x4& proj)
{
    program()->setUniformValue(cachedUniformLocation("persProj"), proj);
}

void DSMaterialShader::init()
{
    const char* SAMPLERS[GBuffer::TEXTURE_COUNT] = {
        "normalSpecData", "diffuseSpecData", "depthData",
    };

    // Set texture samplers
    for(int i = 0; i < GBuffer::TEXTURE_COUNT; ++i)
    {
        program()->setUniformValue(resolveUniformLocation(SAMPLERS[i]), i);
    }

    int samplesLocation = resolveUniformLocation("samples");
    program()->setUniformValue(samplesLocation, samples_);

    int depthRangeLocation = resolveUniformLocation("depthRange");
    program()->setUniformValue(depthRangeLocation, depthRange_);

    resolveUniformLocation("persProj");

    GLuint lightFunc = resolveSubroutineLocation("nullLight", GL_FRAGMENT_SHADER);
    gl->glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &lightFunc);
}