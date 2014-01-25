//
//  Author   : Matti Määttä
//  Summary  : QmlPresenter offers basic c++ scene and Qt Quick integration.
//             Presenter performs the rendering thread's render loop and handles scene
//             switching.
//

#ifndef QMLPRESENTER_H
#define QMLPRESENTER_H

#include "scenepresenter.h"

#include <memory>

#include <QElapsedTimer>
#include <QVariant>
#include <QString>

// Enable profiling of renderer timings.
#ifndef _DEBUG
#define PROFILING
#endif

namespace Engine {

class ResourceDespatcher;
class BasicSceneManager;
class Renderer;
class DebugRenderer;

namespace Ui {

class SceneController;
class RendererFactory;
class InputState;
class UiController;
class RenderTimeWatcher;
class InputEventListener;

class QmlPresenter : public ScenePresenter
{
    Q_OBJECT

public:
    explicit QmlPresenter(QObject* parent = nullptr);
    virtual ~QmlPresenter();

    virtual void setContext(RendererContext* context);
    virtual void setSceneFactory(SceneFactory* factory);

    InputEventListener* inputListener() const;

signals:
    // Signals UiController to remove all watched values.
    void clearWatchList();

    void watchValue(QString name, qreal value, QString unit);

public slots:
    // Called after the view has finished rendering the scene graph.
    // Precondition: viewInitialized and viewSizeChanged received.
    virtual void renderScene();

    // Called when the view render target size changes.
    virtual void viewSizeChanged(QSize size);

    // Called after OpenGL context has been created and OpenGL calls are safe to be done.
    virtual void initialize();

    virtual void setScene(QString scene);

    // Ui slots
    void tonemapAttributeChanged(QString name, QVariant value);
    void generalAttributeChanged(QString name, QVariant value);

private:
    RendererContext* context_;
    SceneFactory* sceneFactory_;

    std::shared_ptr<RendererFactory> rendererFactory_;
    std::shared_ptr<ResourceDespatcher> despatcher_;
    std::shared_ptr<Renderer> renderer_;
    std::shared_ptr<DebugRenderer> debugRenderer_;
    std::shared_ptr<BasicSceneManager> sceneManager_;
    std::shared_ptr<SceneController> sceneController_;
    std::shared_ptr<InputState> input_;
    std::shared_ptr<RenderTimeWatcher> renderTimeWatcher_;

    QSize viewSize_;
    QSize oldSize_;
    QString scene_;
    float fov_;

    QElapsedTimer frameTimer_;

    void updateView();
    void update();
    void render();
};

}}

#endif // QMLPRESENTER_H