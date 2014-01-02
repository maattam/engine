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
    setUniformValue("normalMatrix", normalMatrix);
}

void DSGeometryShader::setMVP(const QMatrix4x4& mvp)
{
    setUniformValue("MVP", mvp);
}

void DSGeometryShader::setMaterialAttributes(const Material::Attributes& attrib)
{
    setUniformValue("material.diffuseColor", attrib.diffuseColor);
    setUniformValue("material.shininess", attrib.shininess);
    setUniformValue("material.specularIntensity", attrib.specularIntensity);
}

void DSGeometryShader::setHasTangentsAndNormals(bool value)
{
    if(value)
    {
        useSubroutine("tangentPass", "calculateTangent", GL_VERTEX_SHADER);
        useSubroutine("calculateNormal", "calculateBumpedNormal", GL_FRAGMENT_SHADER);
    }

    else
    {
        useSubroutine("tangentPass", "skipTangent", GL_VERTEX_SHADER);
        useSubroutine("calculateNormal", "calculateInterpolatedNormal", GL_FRAGMENT_SHADER);
    }
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
        if(!setUniformValue(SAMPLERS[i], i))
        {
            return false;
        }
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