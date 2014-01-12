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
    
bool RenderStage::setViewport(const QRect& viewport, unsigned int samples)
{
    return renderer_->setViewport(viewport, samples);
}

void RenderStage::setGeometryBatch(RenderQueue* batch)
{
    renderer_->setGeometryBatch(batch);
}

void RenderStage::setLights(const QVector<LightData>& lights)
{
    renderer_->setLights(lights);
}

void RenderStage::setSkyboxTexture(CubemapTexture* skybox)
{
    renderer_->setSkyboxTexture(skybox);
}

void RenderStage::render(Graph::Camera* camera)
{
    renderer_->render(camera);

    emit stageFinished();
}

void RenderStage::setRenderTarget(GLuint fbo)
{
    renderer_->setRenderTarget(fbo);
}