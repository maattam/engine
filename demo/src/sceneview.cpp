#include "sceneview.h"

#include <QCoreApplication>
#include <QSurfaceFormat>
#include <QWheelEvent>
#include <QScreen>
#include <QCursor>
#include <qmath.h>

#include <QTimer> // singleShot

#include "input.h"

#include "entity/camera.h"
#include "forwardrenderer.h"
#include "debugrenderer.h"

#include "basicscene.h"
#include "sponzascene.h"

SceneView::SceneView(QWindow* parent) : QWindow(parent),
    renderer_(nullptr), debugRenderer_(nullptr), frame_(0), context_(nullptr),
    funcs_(nullptr), controller_(nullptr)
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

    QTimer* frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, this, &SceneView::updateTitle);
    frameTimer->start(50);

    input_ = new Input(this);
}

SceneView::~SceneView()
{
    if(controller_ != nullptr)
        delete controller_;

    if(renderer_ != nullptr)
        delete renderer_;

    if(debugRenderer_ != nullptr)
        delete debugRenderer_;
}

void SceneView::update()
{
    qint64 lastUpdate = lastTime_.restart();

    // Elapsed in seconds
    float elapsed = static_cast<float>(lastUpdate) / 1000;

    handleInput();

    if(controller_ != nullptr)
    {
        controller_->update(lastUpdate);
    }

    input_->endFrame();
}

void SceneView::updateTitle()
{
    if(controller_ == nullptr)
        return;

    float frameTime = static_cast<double>(frameTime_.restart()) / 1000;

    QVector3D pos = controller_->playerPosition();
    setTitle(QString("FPS: %1, Eye position: (%2, %3, %4)")
        .arg(QString::number(static_cast<int>(frame_ / frameTime)))
        .arg(QString::number(pos.x()))
        .arg(QString::number(pos.y()))
        .arg(QString::number(pos.z()))
        );

    frame_ = 0;
}

void SceneView::render()
{
    if(controller_ != nullptr)
    {
        if(!(debugRenderer_->flags() & Engine::DebugRenderer::DEBUG_WIREFRAME))
        {
            model_.setView(renderer_);
            controller_->renderScene();
        }
        
        model_.setView(debugRenderer_);
        controller_->renderScene();

        ++frame_;
    }
}

void SceneView::initialize()
{
    setWindowState(Qt::WindowMaximized);
    glViewport(0, 0, width(), height());

    // Initialize renderer
    renderer_ = new Engine::ForwardRenderer(&despatcher_);
    if(!renderer_->setViewport(width(), height(), format().samples()))
    {
        qCritical() << "Failed to initialize renderer!";
        exit(-1);
    }

    debugRenderer_ = new Engine::DebugRenderer(&despatcher_);
    debugRenderer_->setViewport(width(), height(), format().samples(), 0, 0);
    debugRenderer_->setObservable(&model_);

    model_.setView(renderer_);

    // Load scene
    swapScene(new SponzaScene(&despatcher_));
    lastTime_.start();
}

void SceneView::handleInput()
{
    // Swap levels
    if(input_->keyDown(Qt::Key::Key_1))
    {
        input_->keyEvent(Qt::Key::Key_1, false);
        swapScene(new SponzaScene(&despatcher_));
    }

    else if(input_->keyDown(Qt::Key::Key_2))
    {
        input_->keyEvent(Qt::Key::Key_2, false);
        swapScene(new BasicScene(&despatcher_));
    }

    // Renderer flags
    if(input_->keyDown(Qt::Key::Key_F1))
    {
        input_->keyEvent(Qt::Key::Key_F1, false);
        toggleRenderFlag(debugRenderer_, Engine::DebugRenderer::DEBUG_AABB);
    }

    if(input_->keyDown(Qt::Key::Key_F2))
    {
        input_->keyEvent(Qt::Key::Key_F2, false);
        toggleRenderFlag(debugRenderer_, Engine::DebugRenderer::DEBUG_WIREFRAME);
    }

    if(input_->keyDown(Qt::Key::Key_F3))
    {
        input_->keyEvent(Qt::Key::Key_F3, false);
        toggleRenderFlag(renderer_, Engine::ForwardRenderer::RENDER_SHADOWS);
    }
}

void SceneView::toggleRenderFlag(Engine::Renderer* renderer, unsigned int flag)
{
    if(renderer->flags() & flag)
    {
        renderer->setFlags(renderer->flags() & ~flag);
    }

    else
    {
        renderer->setFlags(renderer->flags() | flag);
    }
}

void SceneView::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    if(renderer_ != nullptr)
    {
        glViewport(0, 0, width(), height());

        if(!renderer_->setViewport(width(), height(), format().samples()))
        {
            qCritical() << "Failed to initialize renderer!";
            exit(-1);
        }

        debugRenderer_->setViewport(width(), height(), format().samples(), 0, 0);

        if(controller_ != nullptr)
        {
            controller_->setAspectRatio(static_cast<float>(width()) / height());
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

void SceneView::swapScene(FreeLookScene* scene)
{
    if(controller_ != nullptr)
    {
        delete controller_;
    }

    model_.rootNode()->removeAllChildren();

    qDebug() << "Managed objects: " << despatcher_.numManaged();

    scene->setModel(&model_);
    scene->setInput(input_);
    scene->setAspectRatio(static_cast<float>(width()) / height());
    scene->setFov(75.0f);

    controller_ = scene;
}

void SceneView::keyPressEvent(QKeyEvent* event)
{
    input_->keyEvent(event->key(), true);
}

void SceneView::keyReleaseEvent(QKeyEvent* event)
{
    input_->keyEvent(event->key(), false);
}

void SceneView::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MouseButton::RightButton)
    {
        input_->mouseEvent(Input::KEY_MOUSE_RIGHT, true);
        setCursor(Qt::BlankCursor);
    }
}

void SceneView::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MouseButton::RightButton)
    {
        input_->mouseEvent(Input::KEY_MOUSE_RIGHT, false);
        setCursor(Qt::ArrowCursor);
    }
}

void SceneView::wheelEvent(QWheelEvent* event)
{
    input_->wheelEvent(event->delta());
}