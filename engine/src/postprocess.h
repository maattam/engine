// PostProcess applies a fullscreen postprocessing filter to the output fbo

#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include "renderstage.h"

#include <memory>

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

    virtual void render(Graph::Camera* camera);

    virtual void setRenderTarget(GLuint fbo);

    typedef std::shared_ptr<Effect::Postfx> PostfxPtr;

    // Precondition: Viewport has been set.
    // Postcondition: effect ownership is maintained.
    bool setEffect(const PostfxPtr& effect);

private:
    QRect viewport_;

    QOpenGLFramebufferObjectFormat format_;
    GLuint proxy_;
    GLuint texture_;
    GLuint depth_;

    GLuint out_;
    PostfxPtr effect_;

    void deleteBuffers();
    bool initialiseEffect();
};

}

#endif // POSTPROCESS_H