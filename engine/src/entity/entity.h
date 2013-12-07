// Entity abstraction, which is main component of SceneNode

#ifndef ENTITY_H
#define ENTITY_H

#include <deque>
#include "aabb.h"
#include "../renderqueue.h"

namespace Engine {

class Material;
    
namespace Renderable {
    class Renderable;
}

namespace Graph {
    class SceneNode;
}

namespace Entity {

class Entity
{
public:
    Entity();
    virtual ~Entity() = 0;

    // Called by renderer; entity must add its renderables to the list
    virtual void updateRenderList(RenderQueue& list) {};

    const AABB& boundingBox() const;

    // Updated by SceneGraph when the attached node changes.
    virtual void attach(Graph::SceneNode* node);
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