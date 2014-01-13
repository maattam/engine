#include "renderstage.h"

using namespace Engine;

RenderStage::RenderStage(Renderer* renderer)
    : QObject(), renderer_(renderer)
{
}

RenderStage::~RenderStage()
{
    delete renderer_;
}

void RenderStage::setObservable(SceneObservable* observable)
{
    renderer_->setObservable(observable);
}
    
bool RenderStage::setViewport(const QRect& viewport, unsigned int samples)
{
    return renderer_->setViewport(viewport, samples);
}

void RenderStage::setGeometryBatch(RenderQueue* batch)
{
    renderer_->setGeometryBatch(batch);
}

void RenderStage::setCamera(Graph::Camera* camera)
{
    renderer_->setCamera(camera);
}

void RenderStage::render()
{
    renderer_->render();

    emit stageFinished();
}

void RenderStage::setRenderTarget(GLuint fbo)
{
    renderer_->setRenderTarget(fbo);
}