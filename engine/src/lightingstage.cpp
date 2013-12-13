#include "lightingstage.h"

#include "scene/visiblescene.h"

using namespace Engine;

LightingStage::LightingStage(Renderer* renderer)
    : RenderStage(renderer), scene_(nullptr)
{
}

LightingStage::~LightingStage()
{
}

void LightingStage::setScene(VisibleScene* scene)
{
    RenderStage::setScene(scene);

    scene_ = scene;
}

void LightingStage::render(Entity::Camera* camera)
{
    // Query visible lights during the first rendering stage
    scene_->addVisitor(this);

    // Call the actual renderer
    RenderStage::render(camera);

    scene_->removeVisitor(this);
}