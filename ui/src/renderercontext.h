//
//  Author   : Matti Määttä
//  Summary  : RendererContext manages the threaded renderer OpenGL context
//             and synchronises frames with the main thread.
//

#ifndef RENDERERCONTEXT_H
#define RENDERERCONTEXT_H

#include "common.h"

#include <QObject>
#include <QSurfaceFormat>
#include <QSize>

class QOpenGLContext;
class QOpenGLFramebufferObject;
class QOffscreenSurface;
class QOpenGLDebugLogger;

namespace Engine { namespace Ui {

class RenderTargetItem;

class RendererContext : public QObject
{
    Q_OBJECT

public:
    RendererContext(const QSurfaceFormat& format, QObject* parent = nullptr);
    virtual ~RendererContext();

    // Creates a new OpenGL context and shared it with the UI context.
    // This function has to be called before creating a renderer.
    bool createContext(QOpenGLContext* shareContext);

    // This function creates the frame buffer object and emits renderTargetChanged.
    // Precondition: Context has been created
    bool createRenderTarget(const QSize& size);

    // Makes the OpenGL context current
    bool beginRendering();

    // Synchronizes draw calls and emits renderTargetUpdated.
    void endFrame();

    QOpenGLContext* context() const;
    GLuint renderTarget() const;
    const QSurfaceFormat& format() const;

signals:
    void renderTargetUpdated(void* sync);
    void renderTargetChanged(int textureId, QSize size);

private:
    QSurfaceFormat format_;
    QOpenGLContext* context_;
    QOffscreenSurface* screen_;

    QOpenGLFramebufferObject* fbo_;
    QOpenGLDebugLogger* logger_;
};

}}

#endif // RENDERERCONTEXT_H