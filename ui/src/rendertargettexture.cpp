#include "rendertargettexture.h"

#include <QOpenGLFramebufferObject>

using namespace Engine::Ui;

RenderTargetTexture::RenderTargetTexture(int textureId, const QSize& size)
    : QSGTexture(), textureId_(textureId), size_(size)
{
}

RenderTargetTexture::~RenderTargetTexture()
{
}

void RenderTargetTexture::bind()
{
    glBindTexture(GL_TEXTURE_2D, textureId_);
}

bool RenderTargetTexture::hasAlphaChannel() const
{
    return false;
}

bool RenderTargetTexture::hasMipmaps() const
{
    return false;
}

int RenderTargetTexture::textureId() const
{
    return textureId_;
}

QSize RenderTargetTexture::textureSize() const
{
    return size_;
}