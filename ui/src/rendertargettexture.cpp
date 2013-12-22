#include "rendertargettexture.h"

#include <QOpenGLFramebufferObject>

using namespace Engine::Ui;

RenderTargetTexture::RenderTargetTexture(int textureId, const QSize& size)
    : QSGTexture(), textureId_(textureId), size_(size), sync_(0)
{
}

RenderTargetTexture::~RenderTargetTexture()
{
}

void RenderTargetTexture::bind()
{
    // Wait for renderer thread to finish, otherwise we are going to end up with a broken texture.
    if(sync_ != 0)
    {
        gl->glClientWaitSync(sync_, GL_SYNC_GPU_COMMANDS_COMPLETE, GL_TIMEOUT_IGNORED);
    }

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

void RenderTargetTexture::setSyncObject(GLsync sync)
{
    sync_ = sync;
}