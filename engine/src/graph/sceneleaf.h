// Entity abstraction, which is main component of SceneNode

#ifndef ENTITY_H
#define ENTITY_H

#include "aabb.h"
#include "visitable.h"

#include <QString>

namespace Engine {

class RenderQueue;

namespace Graph {

class SceneNode;

class SceneLeaf : public BaseVisitable
{
public:
    SceneLeaf();
    virtual ~SceneLeaf();

    // Called by renderer; entity must add its renderables to the list
    virtual void updateRenderList(RenderQueue& list) = 0;

    // Returns the axis-aligned bounding box which contains this Entity
    const AABB& boundingBox() const;

    // Updated by SceneGraph when the attached node changes.
    // precondition: node != nullptr
    // postcondition: parentNode() == node
    virtual void attach(Graph::SceneNode* node);

    // Detaches the Entity from node
    // postcondition: parentNode() is nullptr
    virtual void detach();

    // Returns nullptr if the entity isn't attached to a node
    Graph::SceneNode* parentNode() const;

    void setName(const QString& name);
    const QString& name() const;

protected:
    void updateAABB(const AABB& aabb);

private:
    QString name_;
    AABB aabb_;
    Graph::SceneNode* node_;

    SceneLeaf(const SceneLeaf&);
    SceneLeaf& operator=(const SceneLeaf&);
};

}}

#endif //ENTITY_H