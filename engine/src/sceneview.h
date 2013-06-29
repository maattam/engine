#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <GL/glew.h>
#include <QtQuick/QQuickView>

namespace Engine {
    class Renderer;
}

class SceneView : public QQuickView
{
    Q_OBJECT

public:
    SceneView(QWindow* target = nullptr);
    ~SceneView();

public slots:
    void render();
    void initialize();

protected:
    virtual void resizeEvent(QResizeEvent* event);

private:
    Engine::Renderer* renderer_;
};

#endif // SCENEVIEW_H