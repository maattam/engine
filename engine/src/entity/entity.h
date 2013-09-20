// Entity abstraction, which is main component of SceneNode

#ifndef ENTITY_H
#define ENTITY_H

#include <deque>
#include "aabb.h"

namespace Engine {
    
    namespace Renderable {
        class Renderable;
    }

namespace Entity {

typedef std::deque<Renderable::Renderable*> RenderList;

class Entity
{
public:
    Entity();
    virtual ~Entity();

    // Called by renderer; entity must add its renderables to the list
    virtual void updateRenderList(RenderList& list) = 0;

    //virtual const AABB& boundingBox() const;

private:
    Entity(const Entity&);
    Entity& operator=(const Entity&);
};

}}

#endif //ENTITY_H