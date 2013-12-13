// Entity abstraction, which is main component of SceneNode

#ifndef ENTITY_H
#define ENTITY_H

#include "aabb.h"
#include "visitable.h"

namespace Engine {

namespace Graph {
    class SceneNode;
}

class RenderQueue;

namespace Entity {

class Entity : public BaseVisitable
{
public:
    Entity();
    virtual ~Entity();

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

protected:
    void updateAABB(const AABB& aabb);

private:
    AABB aabb_;
    Graph::SceneNode* node_;

    Entity(const Entity&);
    Entity& operator=(const Entity&);
};

}}

#endif //ENTITY_H