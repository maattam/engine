#include "renderqueue.h"

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
    // TODO: Sort by material

    RenderItem item = { modelView_, material, renderable };
    queue_.push_back(item);
}

RenderQueue::RenderList::const_iterator RenderQueue::begin() const
{
    return queue_.begin();
}

RenderQueue::RenderList::const_iterator RenderQueue::end() const
{
    return queue_.end();
}