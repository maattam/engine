#include "sceneview.h"

#include "inputeventlistener.h"
#include "scenepresenter.h"
#include "renderercontext.h"
#include "rendertargetitem.h"

#include <QDebug>

using namespace Engine::Ui;

SceneView::SceneView(QWindow* parent)
    : QQuickView(parent)
{
    setResizeMode(QQuickView::SizeRootObjectToView);
    setPersistentOpenGLContext(true);
    setPersistentSceneGraph(true);
}

SceneView::~SceneView()
{
}

void SceneView::setEventListener(InputEventListener* listener)
{
    if(listener == nullptr)
    {
        return;
    }

    connect(this, &SceneView::keyPressSignal, listener, &InputEventListener::keyPressEvent);
    connect(this, &SceneView::keyReleaseSignal, listener, &InputEventListener::keyReleaseEvent);
    connect(this, &SceneView::mouseMoveSignal, listener, &InputEventListener::mouseMoveEvent);
    connect(this, &SceneView::mousePressSignal, listener, &InputEventListener::mousePressEvent);
    connect(this, &SceneView::mouseReleaseSignal, listener, &InputEventListener::mouseReleaseEvent);
    connect(this, &SceneView::wheelSignal, listener, &InputEventListener::wheelEvent);
}

bool SceneView::setPresenter(ScenePresenter* presenter)
{
    if(presenter == nullptr)
    {
        return false;
    }

    RenderTargetItem* renderTarget = dynamic_cast<RenderTargetItem*>(rootObject());
    if(renderTarget == nullptr)
    {
        qDebug() << "Root object has to be RenderTargetItem";
        return false;
    }

    connect(renderTarget, &RenderTargetItem::sizeChanged, presenter, &ScenePresenter::viewSizeChanged);
    connect(this, &SceneView::frameSwapped, presenter, &ScenePresenter::renderScene);

    // Set initial size
    presenter->viewSizeChanged(QSize(renderTarget->width(), renderTarget->height()));

    return true;
}

bool SceneView::setRendererContext(RendererContext* context)
{
    if(context == nullptr)
    {
        return false;
    }

    RenderTargetItem* renderTarget = dynamic_cast<RenderTargetItem*>(rootObject());
    if(renderTarget == nullptr)
    {
        qDebug() << "Root object has to be RenderTargetItem";
        return false;
    }

    connect(context, &RendererContext::renderTargetChanged, renderTarget, &RenderTargetItem::setTexture);
    connect(context, &RendererContext::renderTargetUpdated, renderTarget, &RenderTargetItem::update);

    return true;
}

void SceneView::keyPressEvent(QKeyEvent* ev)
{
    QQuickView::keyPressEvent(ev);
    emit keyPressSignal(*ev);
}

void SceneView::keyReleaseEvent(QKeyEvent* ev)
{
    QQuickView::keyReleaseEvent(ev);
    emit keyReleaseSignal(*ev);
}

void SceneView::mouseMoveEvent(QMouseEvent* ev)
{
    QQuickView::mouseMoveEvent(ev);
    emit mouseMoveSignal(*ev);
}

void SceneView::mousePressEvent(QMouseEvent* ev)
{
    QQuickView::mousePressEvent(ev);
    emit mousePressSignal(*ev);
}

void SceneView::mouseReleaseEvent(QMouseEvent* ev)
{
    QQuickView::mouseReleaseEvent(ev);
    emit mouseReleaseSignal(*ev);
}

void SceneView::wheelEvent(QWheelEvent* ev)
{
    QQuickView::wheelEvent(ev);
    emit wheelSignal(*ev);
}