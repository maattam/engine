#include "shadowmap.h"

#include <QDebug>

using namespace Engine;

ShadowMapTechnique::ShadowMapTechnique(QOpenGLFunctions_4_2_Core* funcs)
    : Technique(), gl(funcs), fbo_(0), shadowMap_(0)
{
}

ShadowMapTechnique::~ShadowMapTechnique()
{
    destroy();
}

void ShadowMapTechnique::destroy()
{
    if(fbo_ != 0)
    {
        gl->glDeleteFramebuffers(1, &fbo_);
    }

    if(shadowMap_ != 0)
    {
        gl->glDeleteTextures(1, &shadowMap_);
    }
}

bool ShadowMapTechnique::init(unsigned int width, unsigned int height)
{
    destroy();

    // Initialize depth texture
    gl->glGenTextures(1, &shadowMap_);
    gl->glBindTexture(GL_TEXTURE_2D, shadowMap_);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Initialize framebuffer
    gl->glGenFramebuffers(1, &fbo_);
    gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
    gl->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap_, 0);
    gl->glDrawBuffer(GL_NONE);

    if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qDebug() << "Failed to init ShadowMapTechnique fbo!";
        return false;
    }

    if(program_.isLinked())
        return true;

    // Compile and link program
    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shadowmap.vert");
    return program_.link();
}

void ShadowMapTechnique::enable()
{
    Technique::enable();

    gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
}

void ShadowMapTechnique::bindTexture(GLenum textureUnit)
{
    gl->glActiveTexture(textureUnit);
    gl->glBindTexture(GL_TEXTURE_2D, shadowMap_);
}

void ShadowMapTechnique::setMVP(const QMatrix4x4& mvp)
{
    program_.setUniformValue("gMVP", mvp);
}