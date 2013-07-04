#include "sceneview.h"

#include <QCoreApplication>
#include <QSurfaceFormat>
#include <QWheelEvent>
#include <QTimer>
#include <QScreen>
#include <QCursor>

#include "engine/camera.h"
#include "engine/renderer.h"

SceneView::SceneView(QWindow* parent) : QWindow(parent),
    renderer_(nullptr), frame_(0), context_(nullptr), funcs_(nullptr),
    renderTimer_(nullptr), scene_(nullptr)
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
    if(scene_ != nullptr)
        delete scene_;

    if(renderer_ != nullptr)
        delete renderer_;
}

void SceneView::update()
{
    if(renderer_ == nullptr)
        return;

    // Elapsed in seconds
    float elapsed = static_cast<float>(lastTime_.restart()) / 1000;

    handleInput(elapsed);
    scene_->update(elapsed);

    // Update title on every tenth frame
    if(frame_ % 10)
    {
        setTitle(QString("Okay Engine -- FPS: ") + QString::number(1.0f / elapsed, 'g', 3));
    }
}

void SceneView::render()
{
    renderer_->render(scene_);

    ++frame_;
}

void SceneView::initialize()
{
    setWindowState(Qt::WindowMaximized);
    glViewport(0, 0, width(), height());

    // Load scene
    scene_ = new BasicScene;
    scene_->initialize(funcs_);

    // Initialize renderer
    renderer_ = new Engine::Renderer(funcs_);

    if(!renderer_->initialize(width(), height(), format().samples()))
    {
        qDebug() << "Failed to initialize renderer!";
    }

    renderer_->prepareScene(scene_);

    scene_->activeCamera()->setAspectRatio(static_cast<float>(width()) / height());

    renderTimer_ = new QTimer(this);
    connect(renderTimer_, SIGNAL(timeout()), this, SLOT(renderNow()));

    // Lock scene update rate to screen refresh rate
    renderTimer_->start(static_cast<qreal>(1000) / screen()->refreshRate() / 2);
    lastTime_.start();
}

void SceneView::handleInput(float elapsed)
{
    const float speed = 10.0f;
    const float mouseSpeed = 0.20f;

    Engine::Camera* camera = scene_->activeCamera();

    if(getKey(Qt::Key::Key_W))
    {
        camera->move(camera->direction() * elapsed * speed);
    }

    if(getKey(Qt::Key::Key_S))
    {
        camera->move(-1.0f * camera->direction() * elapsed * speed);
    }

    if(getKey(Qt::Key::Key_D))
    {
        camera->move(camera->right() * elapsed * speed);
    }

    if(getKey(Qt::Key::Key_A))
    {
        camera->move(-1.0f * camera->right() * elapsed * speed);
    }

    if(getKey(KEY_MOUSE_RIGHT))
    {
        QPoint delta = lastMouse_ - mapFromGlobal(QCursor::pos());
        QCursor::setPos(mapToGlobal(lastMouse_));

        camera->tilt(mouseSpeed * elapsed * delta.x(), mouseSpeed * elapsed * delta.y());
    }
}

void SceneView::wheelEvent(QWheelEvent* event)
{
    float scale = event->delta() / 100.0f;
    Engine::Camera* camera = scene_->activeCamera();

    camera->move(QVector3D::crossProduct(camera->direction(), camera->right()) * scale);

    event->accept();
}

void SceneView::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    if(renderer_ != nullptr)
    {
        glViewport(0, 0, width(), height());

        if(!renderer_->initialize(width(), height(), format().samples()))
        {
            qDebug() << "Failed to initialize renderer!";
        }

        if(scene_ != nullptr)
        {
            scene_->activeCamera()->setAspectRatio(static_cast<float>(width()) / height());
        }
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