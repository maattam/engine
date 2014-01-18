#include "singleshadowmap.h"

using namespace Engine;

SingleShadowMap::SingleShadowMap()
    : fbo_(0)
{
}

SingleShadowMap::~SingleShadowMap()
{
    if(fbo_ != 0)
    {
        gl->glDeleteFramebuffers(1, &fbo_);
    }
}

bool SingleShadowMap::bindTextures(GLenum location)
{
    return texture_.bind(location);
}

void SingleShadowMap::setSize(const QSize& size)
{
    size_ = size;
}

const QSize& SingleShadowMap::size()
{
    return size_;
}

bool SingleShadowMap::create()
{
    if(fbo_ != 0)
    {
        gl->glDeleteFramebuffers(1, &fbo_);
        fbo_ = 0;
    }

    // Create depth texture
    if(!texture_.create(0, GL_DEPTH_COMPONENT16, size_.width(), size_.height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT))
    {
        return false;
    }

    texture_.bind();
    texture_.setFiltering(GL_LINEAR, GL_LINEAR);

    // Clamp texture to white border so sampling outside the texture returns far plane depth value.
    const float color[4] = { 1.0f };
    texture_.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    gl->glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

    // Nvidia PCF shadow2DProj
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    // Create framebuffer
    gl->glGenFramebuffers(1, &fbo_);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_.handle(), 0);
    gl->glDrawBuffer(GL_NONE);

    if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qWarning() << __FUNCTION__ << "Failed to init ShadowMap fbo!";
        return false;
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gl->glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

bool SingleShadowMap::bindFbo()
{
    if(fbo_ == 0)
    {
        return false;
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    return true;
}

GLuint SingleShadowMap::fboHandle() const
{
    return fbo_;
}

const QMatrix4x4& SingleShadowMap::lightVP() const
{
    return lightVP_;
}

void SingleShadowMap::setLightVP(const QMatrix4x4& vp)
{
    lightVP_ = vp;
}

RenderQueue& SingleShadowMap::batch()
{
    return queue_;
}