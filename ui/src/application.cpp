#include "application.h"

#include "sceneview.h"
#include "renderercontext.h"
#include "scenepresenter.h"

#include <QOpenGLContext>
#include <QThread>
#include <QDebug>

using namespace Engine::Ui;

Application::Application(const QSurfaceFormat& format)
    : QObject(), format_(format), thread_(nullptr), view_(nullptr), presenter_(nullptr), context_(nullptr)
{
}

Application::~Application()
{
    cleanup();
}

void Application::setView(SceneView* view)
{
    view_ = view;
    Q_ASSERT(view_);

    connect(view, &SceneView::sceneGraphInitialized, this, &Application::viewInitialized);
    connect(view, &SceneView::sceneGraphInvalidated, this, &Application::viewInvalidated);
}

SceneView* Application::view() const
{
    return view_;
}

void Application::viewInitialized()
{
    Q_ASSERT(view_);

    context_ = new RendererContext(format_);
    if(!context_->createContext(view_->openglContext()))
    {
        return;
    }

    presenter_ = createPresenter();
    Q_ASSERT(presenter_);

    presenter_->setContext(context_);

    if(!view_->setRendererContext(context_))
    {
        qDebug() << "Failed to set renderer context";
        return;
    }

    if(!view_->setPresenter(presenter_))
    {
        qDebug() << "Failed to set presenter";
        return;
    }

    thread_ = new QThread(this);
    thread_->setObjectName("Renderer Thread");

    // Move presenter and renderer context to the rendering thread.
    presenter_->moveToThread(thread_);
    context_->moveToThread(thread_);

    connect(presenter_, &QObject::destroyed, this, &Application::presenterDestroyed);
    QMetaObject::invokeMethod(presenter_, "initialize");

    thread_->start();
}

void Application::viewInvalidated()
{
    if(presenter_ != nullptr)
    {
        QMetaObject::invokeMethod(presenter_, "deleteLater");
    }

    else
    {
        cleanup();
    }
}

void Application::presenterDestroyed(QObject*)
{
    presenter_ = nullptr;
    cleanup();
}

void Application::cleanup()
{
    // Stop rendering thread event loop
    if(thread_ != nullptr)
    {
        thread_->quit();
        thread_->wait();
    }

    // Clean up resources
    if(context_ != nullptr)
    {
        delete context_;
        context_ = nullptr;
    }
}