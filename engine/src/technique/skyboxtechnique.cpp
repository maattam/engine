#include "skyboxtechnique.h"

#include "resourcedespatcher.h"
#include "shader.h"

#include "common.h"

using namespace Engine;
using namespace Engine::Technique;

Skybox::Skybox(ResourceDespatcher* despatcher)
    : Technique(), mvpLocation_(-1), samplerLocation_(-1)
{
    addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/skybox.vert"), Shader::Type::Vertex));
    addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/skybox.frag"), Shader::Type::Fragment));
}

void Skybox::init()
{
    mvpLocation_ = program()->uniformLocation("gMVP");
    samplerLocation_ = program()->uniformLocation("gCubemapTexture");
}

void Skybox::setMVP(const QMatrix4x4& mvp)
{
    program()->setUniformValue(mvpLocation_, mvp);
}

void Skybox::setTextureUnit(unsigned int unit)
{
    program()->setUniformValue(samplerLocation_, unit);
}