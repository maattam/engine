#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <QWindow>
#include "common.h"
#include <QOpenGLContext>
#include <QElapsedTimer>
#include <QPoint>

#include <map>
#include <vector>

#include "basicscene.h"
#include "resourcedespatcher.h"

namespace Engine {
    class Renderer;
}

class SceneView : public QWindow
{
    Q_OBJECT

public:
    enum { KEY_MOUSE_RIGHT = -100 };

    explicit SceneView(QWindow* parent = nullptr);
    ~SceneView();

public slots:
    void renderNow();

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
    void handleInput(float elapsed);

    bool getKey(int key) const;

    Engine::Renderer* renderer_;
    Engine::ResourceDespatcher despatcher_;
    BasicScene* scene_;

    unsigned int frame_;
    QPoint lastMouse_;
    std::map<int, bool> keyMap_;

    QOpenGLContext* context_;
    QOPENGL_FUNCTIONS* funcs_;

    QElapsedTimer lastTime_;
};

#endif // SCENEVIEW_H