#include "sceneview.h"

#include "engine/renderer.h"

#include <QCoreApplication>
#include <QSurfaceFormat>
#include <QResizeEvent>
#include <QTimer>
#include <QScreen>
#include <QCursor>

SceneView::SceneView(QWindow* parent) : QWindow(parent),
    renderer_(nullptr), camera_(QVector3D(0, 0, 5), 3.14f, 0.0f, 45.0f, 0.0f),
    frame_(0), context_(nullptr), funcs_(nullptr), renderTimer_(nullptr)
{
    setSurfaceType(QSurface::OpenGLSurface);

    // Create OpenGL 4.2 context
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
    {
        delete renderer_;
        renderer_ = nullptr;
    }
}

void SceneView::update()
{
    // Elapsed in seconds
    float elapsed = static_cast<float>(lastTime_.restart()) / 1000;
    const float speed = 3.0f;
    const float mouseSpeed = 0.15f;

    renderer_->rotateModel(-15.0f * elapsed);

    if(getKey(Qt::Key::Key_W))
    {
        camera_.move(camera_.direction() * elapsed * speed);
    }

    if(getKey(Qt::Key::Key_S))
    {
        camera_.move(-1.0f * camera_.direction() * elapsed * speed);
    }

    if(getKey(Qt::Key::Key_D))
    {
        camera_.move(camera_.right() * elapsed * speed);
    }

    if(getKey(Qt::Key::Key_A))
    {
        camera_.move(-1.0f * camera_.right() * elapsed * speed);
    }

    if(getKey(KEY_MOUSE_RIGHT))
    {
        QPoint delta = lastMouse_ - mapFromGlobal(QCursor::pos());
        QCursor::setPos(mapToGlobal(lastMouse_));

        camera_.tilt(mouseSpeed * elapsed * delta.x(), mouseSpeed * elapsed * delta.y());
    }
}

void SceneView::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer_->render(camera_);

    ++frame_;
}

void SceneView::initialize()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, width(), height());

    lastTime_.start();

    // Initialize renderer
    renderer_ = new Engine::Renderer(*funcs_);
    camera_.setAspectRatio(static_cast<float>(width()) / height());

    renderTimer_ = new QTimer(this);
    connect(renderTimer_, SIGNAL(timeout()), this, SLOT(renderNow()));

    // Lock scene update rate to screen resfresh rate
    renderTimer_->start(static_cast<qreal>(1000) / screen()->refreshRate());
}

void SceneView::wheelEvent(QWheelEvent* event)
{
    float scale = event->delta() / 1000.0f;
    camera_.move(camera_.direction() * scale);

    event->accept();
}

void SceneView::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    if(renderer_ != nullptr)
    {
        glViewport(0, 0, width(), height());
        camera_.setAspectRatio(static_cast<float>(width()) / height());
    }
}

void SceneView::exposeEvent(QExposeEvent* event)
{
    Q_UNUSED(event);

    if(isExposed())
    {
        renderNow();
    }
}

bool SceneView::event(QEvent* event)
{
    if(event->type() == QEvent::UpdateRequest)
    {
        renderNow();
        return true;
    }

    else
    {
        return QWindow::event(event);
    }
}

void SceneView::renderNow()
{
    update();

    if(!isExposed())
        return;

    bool needsInitialize = false;

    if(context_ == nullptr)
    {
        context_ = new QOpenGLContext(this);
        context_->setFormat(requestedFormat());
        context_->create();

        needsInitialize = true;
    }

    context_->makeCurrent(this);

    if(needsInitialize)
    {
        funcs_ = context_->versionFunctions<QOpenGLFunctions_4_2_Core>();
        if(funcs_ == nullptr)
        {
            qWarning() << "Could not obtain OpenGL context version funtions";
        }

        funcs_->initializeOpenGLFunctions();
        initialize();
    }

    render();

    context_->swapBuffers(this);

    // Disgusting hack to force redraw..
    QCoreApplication::postEvent(this, new QEvent(QEvent::Resize));
}

void SceneView::keyPressEvent(QKeyEvent* event)
{
    keyMap_[event->key()] = true;
}

void SceneView::keyReleaseEvent(QKeyEvent* event)
{
    keyMap_[event->key()] = false;
}

bool SceneView::getKey(int key) const
{
    auto iter = keyMap_.find(key);
    if(iter != keyMap_.end() && iter->second)
    {
        return true;
    }

    return false;
}

void SceneView::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MouseButton::RightButton)
    {
        keyMap_[KEY_MOUSE_RIGHT] = true;
        lastMouse_ = mapFromGlobal(QCursor::pos());
        setCursor(Qt::BlankCursor);
    }
}

void SceneView::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MouseButton::RightButton)
    {
        keyMap_[KEY_MOUSE_RIGHT] = false;
        setCursor(Qt::ArrowCursor);
    }
}