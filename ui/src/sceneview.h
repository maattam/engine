// Basic Engine view with Qt Quick 2 overlay.

#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <QQuickView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

namespace Engine { namespace Ui {

class InputEventListener;
class ScenePresenter;
class RendererContext;

class SceneView : public QQuickView
{
    Q_OBJECT

public:
    explicit SceneView(QWindow* parent = nullptr);
    virtual ~SceneView();

    void setEventListener(InputEventListener* listener);

    // Connects the presenter signals to the RenderTargetItem.
    // Precondition: QQuickItem root object has been set.
    // The root object has to be RenderTargetItem, otherwise this function will fail.
    bool setPresenter(ScenePresenter* presenter);

    // Connects the renderer context signals to the RenderTargetItem.
    // Precondition: QQuickItem root object has been set.
    // The root object has to be RenderTargetItem, otherwise this function will fail.
    bool setRendererContext(RendererContext* context);

signals:
    void keyPressSignal(QKeyEvent ev);
    void keyReleaseSignal(QKeyEvent ev);
    void mouseMoveSignal(QMouseEvent ev);
    void mousePressSignal(QMouseEvent ev);
    void mouseReleaseSignal(QMouseEvent ev);
    void wheelSignal(QWheelEvent ev);

protected:
    virtual void keyPressEvent(QKeyEvent* ev);
    virtual void keyReleaseEvent(QKeyEvent* ev);

    virtual void mouseMoveEvent(QMouseEvent* ev);
    virtual void mousePressEvent(QMouseEvent* ev);
    virtual void mouseReleaseEvent(QMouseEvent* ev);

    virtual void wheelEvent(QWheelEvent* ev);
};

}}

#endif // SCENEVIEW_H