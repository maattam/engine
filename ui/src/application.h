// Application is the entry point of Engine apps.
// Application creates the rendering OpenGL context and thread when the main
// OpenGL context is created.

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QSurfaceFormat>

class QThread;

namespace Engine { namespace Ui {

class ScenePresenter;
class RendererContext;
class SceneView;

class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application(const QSurfaceFormat& format);
    virtual ~Application();

    void setView(SceneView* view);

    SceneView* view() const;

public slots:
    // Called when the view's OpenGL context is ready.
    virtual void viewInitialized();

    // Called when the view's OpenGL context has been destroyed.
    virtual void viewInvalidated();

    virtual void presenterDestroyed(QObject* object);

protected:
    virtual ScenePresenter* createPresenter() = 0;

private:
    QSurfaceFormat format_;
    QThread* thread_;

    SceneView* view_;
    ScenePresenter* presenter_;
    RendererContext* context_;

    void cleanup();
};

}}

#endif // APPLICATION_H