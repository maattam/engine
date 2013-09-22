#include "skyboxtechnique.h"

using namespace Engine::Technique;

Skybox::Skybox() : Technique()
{
    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/skybox.vert");
    program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/skybox.frag");

    if(!program_.link())
    {
        qCritical() << __FUNCTION__ << "Failed to link shaders.";
    }

    mvpLocation_ = program_.uniformLocation("gMVP");
    samplerLocation_ = program_.uniformLocation("gCubemapTexture");
}

void Skybox::setMVP(const QMatrix4x4& mvp)
{
    program_.setUniformValue(mvpLocation_, mvp);
}

void Skybox::setTextureUnit(unsigned int unit)
{
    program_.setUniformValue(samplerLocation_, unit);
}