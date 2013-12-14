// PostProcess applies a fullscreen postprocessing filter to the output fbo

#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include "renderstage.h"

#include <QOpenGLFramebufferObjectFormat>

namespace Engine {

namespace Effect {
    class Postfx;
};

class PostProcess : public RenderStage
{
public:
    // The default framebuffer format is GL_RGBA16F without attachments.
    explicit PostProcess(Renderer* renderer);
    PostProcess(Renderer* renderer, const QOpenGLFramebufferObjectFormat& format);

    virtual ~PostProcess();

    virtual bool setViewport(const QRect& viewport, unsigned int samples);

    virtual void render(Entity::Camera* camera);

    virtual void setOutputFBO(QOpenGLFramebufferObject* fbo);

    // Precondition: Viewport has been set
    bool setEffect(Effect::Postfx* effect);

private:
    QRect viewport_;
    unsigned int samples_;

    QOpenGLFramebufferObjectFormat format_;

    QOpenGLFramebufferObject* proxy_;
    QOpenGLFramebufferObject* out_;

    Effect::Postfx* effect_;
};

}

#endif // POSTPROCESS_H