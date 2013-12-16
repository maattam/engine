#include "renderstage.h"

using namespace Engine;

RenderStage::RenderStage(Renderer* renderer)
    : renderer_(renderer)
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

void RenderStage::setScene(VisibleScene* scene)
{
    renderer_->setScene(scene);
}

void RenderStage::render(Entity::Camera* camera)
{
    renderer_->render(camera);
}

void RenderStage::setOutputFBO(GLuint fbo)
{
    renderer_->setOutputFBO(fbo);
}