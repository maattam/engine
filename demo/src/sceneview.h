#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <QWindow>
#include "common.h"
#include <QOpenGLContext>
#include <QElapsedTimer>
#include <QDebug>

#include "resourcedespatcher.h"
#include "scene/scene.h"

#include "sponzascene.h"
#include "basicscene.h"

namespace Engine {
    class Renderer;
}

class SceneView : public QWindow
{
    Q_OBJECT

public:
    explicit SceneView(QWindow* parent = nullptr);
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

    QOpenGLContext* context_;
    QOPENGL_FUNCTIONS* funcs_;

    Input* input_;

    Engine::Renderer* renderer_;
    Engine::ResourceDespatcher despatcher_;

    Engine::Scene model_;
    FreeLookScene* controller_;

    unsigned int frame_;

    QElapsedTimer lastTime_;
    QElapsedTimer frameTime_;
};

#endif // SCENEVIEW_H