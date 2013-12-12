#include "dsgeometryshader.h"

using namespace Engine;
using namespace Engine::Technique;

DSGeometryShader::DSGeometryShader()
    : Technique()
{
}

DSGeometryShader::~DSGeometryShader()
{
}

void DSGeometryShader::setNormalMatrix(const QMatrix3x3& normalMatrix)
{
    program()->setUniformValue(cachedUniformLocation("normalMatrix"), normalMatrix);
}

void DSGeometryShader::setMVP(const QMatrix4x4& mvp)
{
    program()->setUniformValue(cachedUniformLocation("MVP"), mvp);
}

void DSGeometryShader::setMaterialAttributes(const Material::Attributes& attrib)
{
    program()->setUniformValue(cachedUniformLocation("material.diffuseColor"), attrib.diffuseColor);
    program()->setUniformValue(cachedUniformLocation("material.shininess"), attrib.shininess);
    program()->setUniformValue(cachedUniformLocation("material.specularIntensity"), attrib.specularIntensity);
}

void DSGeometryShader::setHasTangentsAndNormals(bool value)
{
    GLuint fragLoc = -1;
    GLuint vertLoc = -1;

    if(value)
    {
        vertLoc = cachedSubroutineLocation("calculateTangent");
        fragLoc = cachedSubroutineLocation("calculateBumpedNormal");
    }

    else
    {
        vertLoc = cachedSubroutineLocation("skipTangent");
        fragLoc = cachedSubroutineLocation("calculateInterpolatedNormal");
    }

    gl->glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &vertLoc);
    gl->glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &fragLoc);
}

bool DSGeometryShader::init()
{
    const char* SAMPLERS[Material::TEXTURE_COUNT] = {
        "material.diffuseSampler", "material.normalSampler",
        "material.specularSampler", "material.maskSampler"
    };

    // Set texture units
    for(int i = 0; i < Material::TEXTURE_COUNT; ++i)
    {
        program()->setUniformValue(resolveUniformLocation(SAMPLERS[i]), i);
    }

    // Register uniforms
    resolveUniformLocation("normalMatrix");
    resolveUniformLocation("MVP");

    resolveUniformLocation("material.diffuseColor");
    resolveUniformLocation("material.shininess");
    resolveUniformLocation("material.specularIntensity");

    // Subroutine indices
    resolveSubroutineLocation("skipTangent", GL_VERTEX_SHADER);
    resolveSubroutineLocation("calculateTangent", GL_VERTEX_SHADER);

    resolveSubroutineLocation("calculateBumpedNormal", GL_FRAGMENT_SHADER);
    resolveSubroutineLocation("calculateInterpolatedNormal", GL_FRAGMENT_SHADER);

    return true;
}