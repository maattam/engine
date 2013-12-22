#include "demopresenter.h"

#include "weakresourcedespatcher.h"
#include "scene/scene.h"
#include "rendererfactory.h"
#include "renderercontext.h"
#include "renderer.h"
#include "sponzascene.h"
#include "debugrenderer.h"
#include "inputstate.h"

#include <QOpenGLFRamebufferObject>
#include <QDebug>
#include <QThread>

namespace {
    unsigned int toggleRenderFlag(unsigned int current, unsigned int bits);
}

DemoPresenter::DemoPresenter(QObject* parent)
    : ScenePresenter(parent), context_(nullptr)
{
    input_.reset(new InputState);
}

DemoPresenter::~DemoPresenter()
{
}

void DemoPresenter::setContext(Engine::Ui::RendererContext* context)
{
    context_ = context;
}

Engine::Ui::InputEventListener* DemoPresenter::inputListener() const
{
    return input_.get();
}

void DemoPresenter::renderScene()
{
    updateView();

    if(renderer_ == nullptr)
    {
        renderer_.reset(factory_->create());
        renderer_->setViewport(QRect(QPoint(0, 0), viewSize_), context_->format().samples());
        renderer_->setRenderTarget(context_->renderTarget());
        renderer_->setScene(sceneModel_.get());

        debugRenderer_->setGBuffer(factory_->gbuffer());
    }

    render();
    context_->endFrame();

    update();
}

void DemoPresenter::viewSizeChanged(QSize size)
{
    viewSize_ = size;
}

void DemoPresenter::initialize()
{
    qDebug() << "Render thread:" << QThread::currentThreadId();

    if(!context_->beginRendering())
    {
        qDebug() << "Context not ready";
        return;
    }

    despatcher_.reset(new Engine::WeakResourceDespatcher(2));
    factory_.reset(new RendererFactory(*despatcher_));
    sceneModel_.reset(new Engine::Scene());

    sceneController_.reset(new SponzaScene(despatcher_.get()));
    sceneController_->setModel(sceneModel_.get());
    sceneController_->setFov(75.0f);
    sceneController_->setAspectRatio(static_cast<float>(viewSize_.width()) / viewSize_.height());
    sceneController_->setInput(input_.get());

    debugRenderer_.reset(new Engine::DebugRenderer(despatcher_.get()));
    debugRenderer_->setScene(sceneModel_.get());
    debugRenderer_->setObservable(sceneModel_.get());
}

void DemoPresenter::render()
{
    renderer_->render(sceneModel_->camera());
    debugRenderer_->render(sceneModel_->camera());
}

void DemoPresenter::updateView()
{
    if(oldSize_ != viewSize_)
    {
        oldSize_ = viewSize_;
        context_->createRenderTarget(viewSize_);

        if(renderer_ != nullptr)
        {
            renderer_->setViewport(QRect(QPoint(0, 0), viewSize_), context_->format().samples());
            renderer_->setRenderTarget(context_->renderTarget());
        }
        
        debugRenderer_->setViewport(QRect(QPoint(0, 0), viewSize_), context_->format().samples());
        debugRenderer_->setRenderTarget(context_->renderTarget());

        sceneController_->setAspectRatio(static_cast<float>(viewSize_.width()) / viewSize_.height());
    }
}

void DemoPresenter::update()
{
    sceneController_->update(frameTimer_.restart());
    sceneModel_->update();

    if(input_->keyDown(Qt::Key_G))
    {
        input_->setKey(Qt::Key_G, false);
        debugRenderer_->setFlags(toggleRenderFlag(debugRenderer_->flags(), Engine::DebugRenderer::DEBUG_GBUFFER));
    }
}

namespace {
    unsigned int toggleRenderFlag(unsigned int flags, unsigned int bits)
    {
        if(flags & bits)
        {
            return flags & ~bits;
        }

        return flags | bits;
    }
}