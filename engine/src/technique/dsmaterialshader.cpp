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

void DSMaterialShader::setLightDirection(const QVector3D& dir)
{
    program()->setUniformValue("lightDirection", dir.normalized());
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
        GLuint id = resolveUniformLocation(texture + "Data");
        if(id == GL_INVALID_INDEX)
        {
            return false;
        }

        program()->setUniformValue(id, i++);
    }

    // The other uniforms are not critical as they can be replaced by subclasses
    int samplesLocation = resolveUniformLocation("samples");
    program()->setUniformValue(samplesLocation, samples_);

    int depthRangeLocation = resolveUniformLocation("depthRange");
    program()->setUniformValue(depthRangeLocation, depthRange_);

    resolveUniformLocation("persProj");

    GLuint lightFunc = resolveSubroutineLocation("nullLight", GL_FRAGMENT_SHADER);
    gl->glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &lightFunc);

    return true;
}