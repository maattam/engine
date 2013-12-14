#include "postprocess.h"

#include "effect/postfx.h"

#include <QOpenGLFramebufferObject>
#include <QDebug>

using namespace Engine;

PostProcess::PostProcess(Renderer* renderer)
    : RenderStage(renderer), effect_(nullptr), proxy_(nullptr), out_(nullptr), samples_(1)
{
    format_.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    format_.setInternalTextureFormat(GL_RGBA16F);
}

PostProcess::PostProcess(Renderer* renderer, const QOpenGLFramebufferObjectFormat& format)
    : RenderStage(renderer), effect_(nullptr), proxy_(nullptr), out_(nullptr), samples_(1), format_(format)
{
}

PostProcess::~PostProcess()
{
    if(proxy_ != nullptr)
    {
        delete proxy_;
    }
}

bool PostProcess::setViewport(const QRect& viewport, unsigned int samples)
{
    if(!RenderStage::setViewport(viewport, samples))
    {
        return false;
    }

    viewport_ = viewport;
    samples_ = samples;

    if(proxy_ != nullptr)
    {
        delete proxy_;
        proxy_ = nullptr;
    }

    proxy_ = new QOpenGLFramebufferObject(viewport.width(), viewport.height(), format_);
    if(!proxy_->isValid())
    {
        qDebug() << "Failed to create framebuffer object for post-processing!";
        return false;
    }

    setEffect(effect_);
    RenderStage::setOutputFBO(proxy_);

    return true;
}

void PostProcess::render(Entity::Camera* camera)
{
    RenderStage::render(camera);

    if(effect_ != nullptr)
    {
        effect_->render();
    }
}

void PostProcess::setOutputFBO(QOpenGLFramebufferObject* fbo)
{
    out_ = fbo;
    effect_->setOutputFbo(out_ != nullptr ? out_->handle() : 0);
}

bool PostProcess::setEffect(Effect::Postfx* effect)
{
    effect_ = effect;
    if(effect_ == nullptr)
    {
        return false;
    }

    effect_->setInputTexture(proxy_->texture());
    effect_->setOutputFbo(out_ != nullptr ? out_->handle() : 0);

    return effect_->initialize(viewport_.width(), viewport_.height(), samples_);
}