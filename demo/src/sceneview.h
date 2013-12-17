#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <QWindow>
#include "common.h"
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QElapsedTimer>
#include <QDebug>

#include "weakresourcedespatcher.h"
#include "scene/scene.h"

class QOpenGLDebugLogger;
class FreeLookScene;
class Input;

namespace Engine {
    class Renderer;
    class DebugRenderer;
    class GBuffer;
}

class SceneView : public QWindow
{
    Q_OBJECT

public:
    explicit SceneView(const QSurfaceFormat& format, QWindow* parent = nullptr);
    ~SceneView();

public slots:
    void renderNow();
    void updateTitle();

protected:
    virtual void resizeEvent(QResizeEvent* event);
    virtual void exposeEvent(QExposeEvent* event);

    virtual void wheelEvent(QWheelEvent* event);

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    void update();
    void render();
    void initialize();
    void handleInput();
    void swapScene(FreeLookScene* scene);
    void swapRenderer();

    bool deferred_;

    QOpenGLContext* context_;
    QOPENGL_FUNCTIONS* funcs_;

    QOpenGLDebugLogger* debugLogger_;

    Input* input_;

    Engine::Renderer* renderer_;
    Engine::DebugRenderer* debugRenderer_;
    Engine::WeakResourceDespatcher despatcher_;
    Engine::GBuffer* gbuffer_;

    Engine::Scene model_;
    FreeLookScene* controller_;

    unsigned int frame_;

    QElapsedTimer lastTime_;
    QElapsedTimer frameTime_;

    void toggleRenderFlag(Engine::DebugRenderer* renderer, unsigned int flag);
};

#endif // SCENEVIEW_H