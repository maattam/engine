#ifndef RENDERQUEUE_H
#define RENDERQUEUE_H

#include <QMatrix4x4>
#include <QVector>

namespace Engine {

class Material;

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

    // Resets the queue's pointer but doesn't deallocate existing RenderItems.
    void reset();

    // Clears the RenderList.
    void clear();

    typedef QVector<RenderItem> RenderList;

    RenderList::const_iterator begin() const;
    RenderList::const_iterator end() const;

private:
    const QMatrix4x4* modelView_;

    RenderList queue_;
    RenderList::iterator current_;
};

}

#endif // RENDERQUEUE_H