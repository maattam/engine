#include "postprocess.h"

#include "effect/postfx.h"

#include <QOpenGLFramebufferObject>
#include <QDebug>

using namespace Engine;

PostProcess::PostProcess(Renderer* renderer)
    : RenderStage(renderer), proxy_(nullptr), out_(nullptr), samples_(1)
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

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    format.setInternalTextureFormat(GL_RGBA16F);

    proxy_ = new QOpenGLFramebufferObject(viewport.width(), viewport.height(), format);
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