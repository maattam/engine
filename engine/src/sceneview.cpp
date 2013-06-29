#include "sceneview.h"

#include "engine/renderer.h"

#include <QtCore/QTimer>
#include <QtGui/QSurfaceFormat>

SceneView::SceneView(QWindow* parent) : QQuickView(parent), renderer_(nullptr)
{
    setClearBeforeRendering(false);
    setResizeMode(QQuickView::SizeRootObjectToView);
    setPersistentOpenGLContext(false);

    resize(500, 400);

    // Render scene before gui
    connect(this, SIGNAL(beforeRendering()), SLOT(render()), Qt::DirectConnection);
    connect(this, SIGNAL(sceneGraphInitialized()), SLOT(initialize()), Qt::DirectConnection);

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(16);

    // Create OpenGL 4.3 context
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);
}

SceneView::~SceneView()
{
    if(renderer_ != nullptr)
        delete renderer_;
}

void SceneView::resizeEvent(QResizeEvent* event)
{
    QQuickView::resizeEvent(event);

    glViewport(0, 0, width(), height());

    event->accept();
}

void SceneView::initialize()
{
    if(renderer_ == nullptr)
        renderer_ = new Engine::Renderer(openglContext());
}

void SceneView::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer_->render();
}
