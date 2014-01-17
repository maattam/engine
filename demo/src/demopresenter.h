//
//  Author   : Matti Määttä
//  Summary  : 
//

#ifndef DEMOPRESENTER_H
#define DEMOPRESENTER_H

#include "scenepresenter.h"

#include <memory>

#include <QElapsedTimer>
#include <QVariant>
#include <QString>

// Enable profiling of renderer timings.
#ifndef _DEBUG
#define PROFILING
#endif

class FreeLookScene;
class RendererFactory;
class SceneFactory;
class InputState;
class UiController;
class RenderTimeWatcher;

namespace Engine {
    class ResourceDespatcher;
    class BasicSceneManager;
    class Renderer;
    class DebugRenderer;

    namespace Ui {
        class InputEventListener;
    }
}

class DemoPresenter : public Engine::Ui::ScenePresenter
{
    Q_OBJECT

public:
    explicit DemoPresenter(QObject* parent = nullptr);
    virtual ~DemoPresenter();

    virtual void setContext(Engine::Ui::RendererContext* context);

    Engine::Ui::InputEventListener* inputListener() const;

    void setUiController(UiController* controller);

public slots:
    // Called after the view has finished rendering the scene graph.
    // Precondition: viewInitialized and viewSizeChanged received.
    virtual void renderScene();

    // Called when the view render target size changes.
    virtual void viewSizeChanged(QSize size);

    // Called after OpenGL context has been created and OpenGL calls are safe to be done.
    virtual void initialize();

    // Ui slots
    void tonemapAttributeChanged(QString name, QVariant value);
    void generalAttributeChanged(QString name, QVariant value);

private:
    Engine::Ui::RendererContext* context_;
    UiController* uiController_;

    std::shared_ptr<RendererFactory> rendererFactory_;
    std::shared_ptr<SceneFactory> sceneFactory_;
    std::shared_ptr<Engine::ResourceDespatcher> despatcher_;
    std::shared_ptr<Engine::Renderer> renderer_;
    std::shared_ptr<Engine::DebugRenderer> debugRenderer_;
    std::shared_ptr<Engine::BasicSceneManager> sceneManager_;
    std::shared_ptr<FreeLookScene> sceneController_;
    std::shared_ptr<InputState> input_;
    std::shared_ptr<RenderTimeWatcher> renderTimeWatcher_;

    QSize viewSize_;
    QSize oldSize_;
    QString scene_;

    QElapsedTimer frameTimer_;

    void updateView();
    void update();
    void render();
};

#endif // DEMOPRESENTER_H