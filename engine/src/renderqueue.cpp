//
//  Author   : Matti Määttä
//  Summary  : 
//

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
    stacks_[material->renderType()].push_back({ modelView_, material, renderable });
}

void RenderQueue::clear()
{
    for(auto& list : stacks_)
    {
        list.clear();
    }

    modelView_ = nullptr;
}

RenderQueue::RenderRange RenderQueue::getItems(Material::RenderType renderIndex)
{
    const RenderList& list = stacks_[renderIndex];
    return qMakePair(list.begin(), list.end());
}