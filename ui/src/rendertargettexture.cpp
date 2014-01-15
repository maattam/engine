#include "rendertargettexture.h"

#include <QOpenGLFramebufferObject>
#include <QDebug>
#include <QThread>

using namespace Engine::Ui;

RenderTargetTexture::RenderTargetTexture()
    : QSGTexture(), textureId_(0), sync_(0)
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
        /*if(gl->glClientWaitSync(sync_, 0, GL_TIMEOUT_IGNORED) == GL_WAIT_FAILED)
        {
            qWarning() << "glClientWaitSync failed";
        }*/

        GLenum result;

        do
        {
            // Prevent 100% core utilisation while waiting for our rendering to finish.
            // NVIDIA drivers implement glClientWaitSync using busy loop.
            result = gl->glClientWaitSync(sync_, 0, 0);
            if(result == GL_TIMEOUT_EXPIRED)
            {
                QThread::msleep(1);
            }

        } while(result == GL_TIMEOUT_EXPIRED);

        gl->glDeleteSync(sync_);
        sync_ = 0;
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

void RenderTargetTexture::update(int textureId, const QSize& size)
{
    textureId_ = textureId;
    size_ = size;
}