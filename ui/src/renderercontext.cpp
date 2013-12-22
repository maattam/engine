#include "renderercontext.h"

#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QDebug>

#include "common.h"

using namespace Engine::Ui;

RendererContext::RendererContext(const QSurfaceFormat& format, QObject* parent)
    : QObject(parent), format_(format), context_(nullptr), screen_(nullptr), fbo_(nullptr), logger_(nullptr), sync_(0)
{
    screen_ = new QOffscreenSurface;
    screen_->setFormat(format);
    screen_->setParent(this);
    screen_->create();
}

RendererContext::~RendererContext()
{
    if(sync_ != 0)
    {
        gl->glDeleteSync(sync_);
    }

    if(fbo_ != nullptr)
    {
        delete fbo_;
    }
}

bool RendererContext::createContext(QOpenGLContext* shareContext)
{
    if(context_ != nullptr)
    {
        delete context_;
    }

    context_ = new QOpenGLContext(this);
    context_->setShareContext(shareContext);
    context_->setFormat(screen_->requestedFormat());

    if(!context_->create())
    {
        qCritical() << "Could not create rendering context";
        return false;
    }

    context_->makeCurrent(screen_);

    gl = context_->versionFunctions<QOPENGL_FUNCTIONS>();
    if(gl == nullptr || !gl->initializeOpenGLFunctions())
    {
        qCritical() << "Could not obtain OpenGL context version functions";
        return false;
    }

    // Enable debug logging when debug context is set
    if(format_.testOption(QSurfaceFormat::DebugContext))
    {
        logger_ = new QOpenGLDebugLogger(this);
        logger_->initialize();
    }

    context_->doneCurrent();
    return true;
}

bool RendererContext::createRenderTarget(const QSize& size)
{
    if(size.width() < 1 || size.height() < 1)
    {
        return false;
    }

    if(fbo_ != nullptr)
    {
        delete fbo_;
    }

    fbo_ = new QOpenGLFramebufferObject(size.width(), size.height());
    if(!fbo_->isValid())
    {
        qWarning() << "Failed to create proxy framebuffer object";
        return false;
    }

    emit renderTargetChanged(fbo_->texture(), fbo_->size());
    return true;
}

bool RendererContext::beginRendering()
{
    if(context_ == nullptr)
    {
        return false;
    }

    return context_->makeCurrent(screen_);
}

void RendererContext::endFrame()
{
    if(logger_ != nullptr)
    {
        for(auto message : logger_->loggedMessages())
        {
            qDebug() << message;
        }
    }

    if(sync_ != 0)
    {
        gl->glDeleteSync(sync_);
    }

    // Synchronize the render target texture between the two renderers
    sync_ = gl->glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    gl->glFlush();

    emit renderTargetUpdated(sync_);
}

QOpenGLContext* RendererContext::context() const
{
    return context_;
}

GLuint RendererContext::renderTarget() const
{
    if(fbo_ == 0)
    {
        return context_->defaultFramebufferObject();
    }

    return fbo_->handle();
}

const QSurfaceFormat& RendererContext::format() const
{
    return format_;
}