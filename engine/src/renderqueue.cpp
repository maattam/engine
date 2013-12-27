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
    int order = RENDER_OPAQUE;

    if(material->attributes().ambientColor != QVector3D(0, 0, 0))
    {
        order = RENDER_EMISSIVE;
    }

    if(material->hasTexture(Material::TEXTURE_NORMALS) && renderable->hasTangents())
    {
        order++;
    }

    queue_.insertMulti(order, item);
}

void RenderQueue::clear()
{
    queue_.clear();
    modelView_ = nullptr;
}

const RenderQueue::RenderList& RenderQueue::queue() const
{
    return queue_;
}

RenderQueue::RenderList::const_iterator RenderQueue::begin() const
{
    return queue_.begin();
}

RenderQueue::RenderList::const_iterator RenderQueue::end() const
{
    return queue_.end();
}