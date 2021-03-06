//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "skybox.h"

#include "cubemaptexture.h"
#include "graph/camera.h"

#include "common.h"

using namespace Engine;
using namespace Engine::Technique;

Skybox::Skybox(unsigned int samples)
    : Technique(), textureUnit_(-1), depthUnit_(-1), samples_(samples), brightness_(1.0f)
{
}

Skybox::~Skybox()
{
}

void Skybox::addShader(const Shader::Ptr& shader)
{
    Technique::addShader(shader);

    if(shader->type() == Shader::Type::Fragment)
    {
        ShaderData::DefineMap defines;
        defines.insert("SAMPLES", samples_);

        shader->setNamedDefines(defines);
    }
}

void Skybox::setTextureUnit(int unit)
{
    textureUnit_ = unit;

    if(program()->isLinked())
    {
        setUniformValue("cubemap", textureUnit_);
    }
}

void Skybox::setDepthTextureUnit(int unit)
{
    depthUnit_ = unit;
}

void Skybox::setMVP(const QMatrix4x4& mvp)
{
    setUniformValue("MVP", mvp);
}

void Skybox::setBrightness(float brightness)
{
    brightness_ = brightness;

    if(program()->isLinked())
    {
        setUniformValue("brightness", brightness_);
    }
}

bool Skybox::init()
{
    resolveUniformLocation("MVP");
    if(!setUniformValue("cubemap", textureUnit_))
    {
        return false;
    }

    if(!setUniformValue("brightness", brightness_))
    {
        return false;
    }

    if(depthUnit_ != -1)
    {
        setUniformValue("depth", depthUnit_);
        useSubroutine("depthTest", "sampleDepthTest", GL_FRAGMENT_SHADER);
    }

    else
    {
        // Kinda hacky since two samplers can't point to the same unit or be invalid (-1).
        // TODO: Implement some sort of #define -switch for shaders.
        setUniformValue("depth", textureUnit_ + 1);
        useSubroutine("depthTest", "skipDepthTest", GL_FRAGMENT_SHADER);
    }

    return true;
}