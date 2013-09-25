#include "skyboxtechnique.h"

#include "resourcedespatcher.h"
#include "resource/shader.h"

using namespace Engine;
using namespace Engine::Technique;

Skybox::Skybox(ResourceDespatcher* despatcher)
    : Technique(), mvpLocation_(-1), samplerLocation_(-1)
{
    addShader(despatcher->get<Shader>(":/shaders/skybox.vert"));
    addShader(despatcher->get<Shader>(":/shaders/skybox.frag"));
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