#include "postprocess.h"

#include "effect/postfx.h"

#include <QOpenGLFramebufferObject>
#include <QDebug>

using namespace Engine;

PostProcess::PostProcess(Renderer* renderer)
    : RenderStage(renderer), proxy_(0), out_(0)
{
    format_.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    format_.setInternalTextureFormat(GL_RGBA16F);
    format_.setSamples(1);

    texture_ = depth_ = 0;
}

PostProcess::PostProcess(Renderer* renderer, const QOpenGLFramebufferObjectFormat& format)
    : RenderStage(renderer), proxy_(0), out_(0), format_(format)
{
    texture_ = depth_ = 0;
}

PostProcess::~PostProcess()
{
    deleteBuffers();
}

void PostProcess::deleteBuffers()
{
    if(texture_ != 0)
        gl->glDeleteTextures(1, &texture_);

    if(depth_ != 0)
        gl->glDeleteTextures(1, &depth_);

    if(proxy_ != 0)
        gl->glDeleteFramebuffers(1, &proxy_);
}

bool PostProcess::setViewport(const QRect& viewport, unsigned int samples)
{
    if(!RenderStage::setViewport(viewport, samples))
    {
        return false;
    }

    viewport_ = viewport;

    deleteBuffers();

    // Create new framebuffer object based on the given format
    gl->glGenFramebuffers(1, &proxy_);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, proxy_);

    // Color attachment
    gl->glGenTextures(1, &texture_);
    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_);
    gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, format_.samples(), format_.internalTextureFormat(),
        viewport.width(), viewport.height(), GL_TRUE);

    gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    gl->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_, 0);

    // Depth attachment
    if(format_.attachment() == QOpenGLFramebufferObject::Depth)
    {
        gl->glGenRenderbuffers(1, &depth_);
        gl->glBindRenderbuffer(GL_RENDERBUFFER, depth_);
        gl->glRenderbufferStorageMultisample(GL_RENDERBUFFER, format_.samples(), GL_DEPTH_COMPONENT32,
            viewport.width(), viewport.height());

        gl->glBindRenderbuffer(GL_RENDERBUFFER, 0);
        gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_);
    }

    // TODO: If stencil is needed in output pass, create it here

    GLenum status = gl->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        qDebug() << "Failed to create framebuffer object for post-processing!";
        return false;
    }

    initialiseEffect();
    RenderStage::setRenderTarget(proxy_);

    return true;
}

void PostProcess::render(Graph::Camera* camera)
{
    RenderStage::render(camera);

    if(effect_ != nullptr)
    {
        effect_->render();
    }
}

void PostProcess::setRenderTarget(GLuint fbo)
{
    out_ = fbo;
    effect_->setOutputFbo(out_);
}

bool PostProcess::setEffect(const PostfxPtr& effect)
{
    effect_ = effect;
    return initialiseEffect();
}

bool PostProcess::initialiseEffect()
{
    if(effect_ == nullptr)
    {
        return false;
    }

    effect_->setInputTexture(texture_);
    effect_->setOutputFbo(out_);

    return effect_->initialize(viewport_.width(), viewport_.height(), format_.samples());
}