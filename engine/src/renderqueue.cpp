#include "renderqueue.h"

using namespace Engine;

RenderQueue::RenderQueue()
    : modelView_(nullptr)
{
    current_ = queue_.begin();
}

void RenderQueue::setModelView(const QMatrix4x4* modelView)
{
    modelView_ = modelView;
}

void RenderQueue::addNode(Material* material, Renderable::Renderable* renderable)
{
    RenderItem item = { modelView_, material, renderable };

    // TODO: Sort by material

    if(current_ == queue_.end())
    {
        queue_.push_back(item);
        current_ = queue_.end();
    }

    else
    {
        *current_++ = item;
    }
}

void RenderQueue::reset()
{
    current_ = queue_.begin();
}

void RenderQueue::clear()
{
    queue_.clear();
    current_ = queue_.begin();
}

RenderQueue::RenderList::const_iterator RenderQueue::begin() const
{
    return queue_.begin();
}

RenderQueue::RenderList::const_iterator RenderQueue::end() const
{
    return current_;
}