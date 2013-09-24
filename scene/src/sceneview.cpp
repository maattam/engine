#include "sceneview.h"

#include <QCoreApplication>
#include <QSurfaceFormat>
#include <QWheelEvent>
#include <QScreen>
#include <QCursor>

#include <QTimer> // singleShot

#include "entity/camera.h"
#include "renderer.h"

SceneView::SceneView(QWindow* parent) : QWindow(parent),
    renderer_(nullptr), frame_(0), context_(nullptr), funcs_(nullptr), scene_(nullptr)
{
    setSurfaceType(QSurface::OpenGLSurface);

    // Create OpenGL 4.2 context
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
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
    qint64 lastUpdate = lastTime_.restart();

    // Elapsed in seconds
    float elapsed = static_cast<float>(lastUpdate) / 1000;

    handleInput(elapsed);
    scene_->update(lastUpdate);

    // Update title on every tenth frame
    if(frame_ % 10)
    {
        QVector3D pos = scene_->activeCamera()->position();
        setTitle(QString("FPS: %1, POS: [%2 %3 %4]").arg(QString::number(1.0f / elapsed, 'g', 3))
            .arg(QString::number(pos.x())).arg(QString::number(pos.y())).arg(QString::number(pos.z())));
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
    scene_ = new BasicScene();
    scene_->initialize();

    // Initialize renderer
    renderer_ = new Engine::Renderer();
    if(!renderer_->initialize(width(), height(), format().samples()))
    {
        qCritical() << "Failed to initialize renderer!";
        exit(-1);
    }

    renderer_->prepareScene(scene_);
    scene_->activeCamera()->setAspectRatio(static_cast<float>(width()) / height());
    lastTime_.start();
}

void SceneView::handleInput(float elapsed)
{
    const float speed = 10.0f;
    const float mouseSpeed = 0.15f;

    if(scene_ == nullptr)
        return;

    Engine::Entity::Camera* camera = scene_->activeCamera();

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
    Engine::Entity::Camera* camera = scene_->activeCamera();

    camera->move(QVector3D(0, scale, 0));

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
            qCritical() << "Failed to initialize renderer!";
            exit(-1);
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
        funcs_ = context_->versionFunctions<QOPENGL_FUNCTIONS>();
        if(funcs_ == nullptr || !funcs_->initializeOpenGLFunctions())
        {
            qCritical() << "Could not obtain OpenGL context version functions";
            exit(-1);
        }

        gl = funcs_;
        initialize();
    }

    update();
    render();

    context_->swapBuffers(this);

    QTimer::singleShot(0, this, SLOT(renderNow()));
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