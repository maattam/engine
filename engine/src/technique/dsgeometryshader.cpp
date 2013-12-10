#include "dsgeometryshader.h"

#include "resourcedespatcher.h"

using namespace Engine;
using namespace Engine::Technique;

DSGeometryShader::DSGeometryShader(ResourceDespatcher* despatcher)
    : Technique(despatcher)
{
}

DSGeometryShader::~DSGeometryShader()
{
}

void DSGeometryShader::setModelViewMatrix(const QMatrix4x4& modelView)
{
    program()->setUniformValue(modelViewLocation_, modelView);
}

void DSGeometryShader::setMVP(const QMatrix4x4& mvp)
{
    program()->setUniformValue(mvpLocation_, mvp);
}

void DSGeometryShader::setMaterialAttributes(const Material::Attributes& attrib)
{
    program()->setUniformValue(materialLocation_.diffuseColor, attrib.diffuseColor);
    program()->setUniformValue(materialLocation_.shininess, attrib.shininess);
    program()->setUniformValue(materialLocation_.specularIntensity, attrib.specularIntensity);
}

void DSGeometryShader::setHasTangentsAndNormals(bool value)
{
    if(value)
    {
        gl->glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &calcTangentIndex_);
        gl->glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &calcNormalIndex_);
    }

    else
    {
        gl->glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &skipTangentIndex_);
        gl->glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &interpNormalIndex_);
    }
}

void DSGeometryShader::init()
{
    const char* SAMPLERS[Material::TEXTURE_COUNT] = {
        "material.diffuseSampler", "material.normalSampler",
        "material.specularSampler", "material.maskSampler"
    };

    // Set texture units
    for(int i = 0; i < Material::TEXTURE_COUNT; ++i)
    {
        program()->setUniformValue(SAMPLERS[i], i);
    }

    // Fetch attribute locations
    modelViewLocation_ = program()->attributeLocation("modelViewMatrix");
    mvpLocation_ = program()->attributeLocation("MVP");

    materialLocation_.diffuseColor = program()->attributeLocation("material.diffuseColor");
    materialLocation_.shininess = program()->attributeLocation("material.shininess");
    materialLocation_.specularIntensity = program()->attributeLocation("material.specularIntensity");

    // Subroutine indices
    skipTangentIndex_ = gl->glGetSubroutineIndex(program()->programId(), GL_VERTEX_SHADER, "skipTangent");
    calcTangentIndex_ = gl->glGetSubroutineIndex(program()->programId(), GL_VERTEX_SHADER, "calculateTangent");

    calcNormalIndex_ = gl->glGetSubroutineIndex(program()->programId(), GL_FRAGMENT_SHADER, "calculateBumpedNormal");
    interpNormalIndex_ = gl->glGetSubroutineIndex(program()->programId(), GL_FRAGMENT_SHADER, "calculateInterpolatedNormal");
}