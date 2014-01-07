#include "renderqueue.h"

#include "material.h"
#include "renderable/renderable.h"

using namespace Engine;

RenderQueue::RenderQueue()
    : modelView_(nullptr)
{
}

void RenderQueue::setModelView(const QMatrix4x4* modelView)
{
    modelView_ = modelView;
}

void RenderQueue::addNode(Material* material, Renderable::Renderable* renderable)
{
    RenderItem item = { modelView_, material, renderable };

    queue_[material->renderType()].push_back(item);
}

void RenderQueue::clear()
{
    queue_.clear();
    modelView_ = nullptr;
}

RenderQueue::RenderRange RenderQueue::getItems(Material::RenderType renderIndex) const
{
    return qMakePair(queue_[renderIndex].begin(), queue_[renderIndex].end());
}