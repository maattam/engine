// Entity abstraction, which is main component of SceneNode

#ifndef ENTITY_H
#define ENTITY_H

#include <deque>
#include "aabb.h"

namespace Engine {

class Material;
    
namespace Renderable {
    class Renderable;
}

typedef std::deque<std::pair<Material*, Renderable::Renderable*>> RenderList;

namespace Entity {

class Entity
{
public:
    Entity();
    virtual ~Entity();

    // Called by renderer; entity must add its renderables to the list
    virtual void updateRenderList(RenderList& list) = 0;

    const AABB& boundingBox() const;

protected:
    void updateAABB(const AABB& aabb);

private:
    Entity(const Entity&);
    Entity& operator=(const Entity&);

    AABB aabb_;
};

}}

#endif //ENTITY_H