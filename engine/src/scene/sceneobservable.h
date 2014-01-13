#ifndef SCENEOBSERVABLE_H
#define SCENEOBSERVABLE_H

#include "sceneobserver.h"
#include "observable.h"

#include <QMatrix4x4>

class BaseVisitor;

namespace Engine {

class RenderQueue;

class SceneObservable : public Observable<SceneObserver>
{
public:
    // Adds a scene leaf visitor, which will be called for culled leaves.
    // If the visitor already exists, it won't be duplicated.
    // precondition: visitor != nullptr
    virtual void addVisitor(BaseVisitor* visitor) = 0;

    // Removes a visitor from the visitor list.
    virtual void removeVisitor(BaseVisitor* visitor) = 0;

    typedef bool (*AcceptVisibleLeaf)(const Graph::SceneLeaf& leaf, const Graph::SceneNode& node);

    // Queries a list of visible scene leaves inside the given frustum. If acceptFunc is not null,
    // the leaf can be rejected by returning false when the function is called.
    virtual void findVisibleLeaves(const QMatrix4x4& frustum, RenderQueue& queue, AcceptVisibleLeaf acceptFunc) = 0;
};

};

#endif // SCENEOBSERVABLE_H