#include "demopresenter.h"

#include "weakresourcedespatcher.h"
#include "scene/basicscenemanager.h"
#include "rendererfactory.h"
#include "scenefactory.h"
#include "renderercontext.h"
#include "renderer.h"
#include "debugrenderer.h"
#include "inputstate.h"
#include "freelookscene.h"

#include "technique/hdrtonemap.h"
#include "effect/hdr.h"

#include <QOpenGLFRamebufferObject>
#include <QDebug>
#include <QThread>

namespace {
    unsigned int toggleRenderFlag(unsigned int current, unsigned int bits);
}

DemoPresenter::DemoPresenter(QObject* parent)
    : ScenePresenter(parent), context_(nullptr), scene_("Sponza")
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
        renderer_.reset(rendererFactory_->create(context_->format().samples()));
        if(!renderer_->setViewport(QRect(QPoint(0, 0), viewSize_), context_->format().samples()))
        {
            qWarning() << "setViewport failed:" << viewSize_;
            return;
        }

        renderer_->setRenderTarget(context_->renderTarget());
        sceneManager_->setRenderer(renderer_.get());

        debugRenderer_->setGBuffer(rendererFactory_->gbuffer());
    }

    if(sceneController_ == nullptr)
    {
        sceneController_.reset(sceneFactory_->create(scene_));
        if(!sceneController_)
        {
            return;
        }

        sceneController_->setManager(sceneManager_.get());
        sceneController_->setFov(75.0f);
        sceneController_->setInput(input_.get());
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
    rendererFactory_.reset(new RendererFactory(*despatcher_));
    sceneFactory_.reset(new SceneFactory(*despatcher_));
    sceneManager_.reset(new Engine::BasicSceneManager());

    debugRenderer_.reset(new Engine::DebugRenderer(despatcher_.get()));
    debugRenderer_->setObservable(sceneManager_.get());
}

void DemoPresenter::render()
{
    if(!(debugRenderer_->flags() & Engine::DebugRenderer::DEBUG_WIREFRAME))
    {
        sceneManager_->renderFrame();
    }

    //debugRenderer_->render(nullptr);
}

void DemoPresenter::updateView()
{
    if(oldSize_ != viewSize_)
    {
        oldSize_ = viewSize_;
        context_->createRenderTarget(viewSize_);

        QRect viewport(QPoint(0, 0), viewSize_);
        sceneManager_->setViewport(viewport);

        if(renderer_ != nullptr)
        {
            renderer_->setViewport(viewport, context_->format().samples());
            renderer_->setRenderTarget(context_->renderTarget());
        }
        
        debugRenderer_->setViewport(viewport, context_->format().samples());
        debugRenderer_->setRenderTarget(context_->renderTarget());
    }
}

void DemoPresenter::update()
{
    sceneController_->update(frameTimer_.restart());
    sceneManager_->prepareNextFrame();
}

void DemoPresenter::tonemapAttributeChanged(QString name, QVariant value)
{
    if(rendererFactory_ == nullptr)
    {
        return;
    }

    if(name == "bloom threshold")
    {
        rendererFactory_->hdr()->setBrightThreshold(value.toFloat());
    }

    else if(name == "bloom factor")
    {
        rendererFactory_->tonemap()->setBloomFactor(value.toFloat());
    }

    else if(name == "bright level")
    {
        rendererFactory_->tonemap()->setBrightLevel(value.toFloat());
    }

    else if(name == "gamma")
    {
        rendererFactory_->tonemap()->setGamma(value.toFloat());
    }

    else if(name == "exposure")
    {
        float exposure = value.toFloat();
        if(exposure > -0.5)
        {
            rendererFactory_->setAutoExposure(false);
            rendererFactory_->tonemap()->setExposure(value.toFloat());
        }

        else
        {
            rendererFactory_->setAutoExposure(true);
        }
    }
}

void DemoPresenter::generalAttributeChanged(QString name, QVariant value)
{
    if(!rendererFactory_ || !sceneController_)
    {
        return;
    }

    if(name == "deferred rendering")
    {
        if(value.toBool())
        {
            rendererFactory_->setRendererType(RendererFactory::DEFERRED);
        }

        else
        {
            rendererFactory_->setRendererType(RendererFactory::FORWARD);
        }

        renderer_.reset();
    }

    else if(name == "scene")
    {
        sceneController_.reset();
        scene_ = value.toString();
    }

    else if(name == "show gbuffer")
    {
        debugRenderer_->setFlags(toggleRenderFlag(debugRenderer_->flags(), Engine::DebugRenderer::DEBUG_GBUFFER));
    }

    else if(name == "show bounding boxes")
    {
        debugRenderer_->setFlags(toggleRenderFlag(debugRenderer_->flags(), Engine::DebugRenderer::DEBUG_AABB));
    }

    else if(name == "show wireframe")
    {
        debugRenderer_->setFlags(toggleRenderFlag(debugRenderer_->flags(), Engine::DebugRenderer::DEBUG_WIREFRAME));
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