#ifndef RENDERQUEUE_H
#define RENDERQUEUE_H

#include <QMatrix4x4>
#include <QMap>
#include <QList>

#include "material.h"

namespace Engine {

namespace Renderable {
    class Renderable;
}



class RenderQueue
{
public:
    RenderQueue();

    struct RenderItem
    {
        const QMatrix4x4* modelView;
        Material* material;
        Renderable::Renderable* renderable;
    };

    // Sets transformation matrix for future addNode calls
    // precondition: modelView != nullptr
    void setModelView(const QMatrix4x4* modelView);

    // Queues a new RenderItem under the set model view matrix
    // precondition: model view matrix set, material != nullptr, renderable != nullptr
    void addNode(Material* material, Renderable::Renderable* renderable);

    // Clears the RenderList.
    void clear();

    // Sorts the RenderItems in each render category.
    // Comparator has to be a less-than functor that compares two RenderItem references.
    template<typename Comparator>
    void sort(const Comparator& lessThan);

    typedef QList<RenderItem> RenderList;
    typedef QPair<RenderList::ConstIterator, RenderList::ConstIterator> RenderRange;

    // Returns iterators to the RenderList based on RenderType.
    RenderRange getItems(Material::RenderType renderIndex);

private:
    typedef QMap<Material::RenderType, RenderList> RenderMap;

    const QMatrix4x4* modelView_;
    RenderMap queue_;
};

#include "renderqueue.inl"

}

#endif // RENDERQUEUE_H