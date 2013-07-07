// Entity abstraction, which is main component of SceneNode

#ifndef ENTITY_H
#define ENTITY_H

#include <list>

namespace Engine {

class Renderable;

class Entity
{
public:
    typedef std::list<Renderable*> RenderList;

    Entity();
    virtual ~Entity();

    // Called by renderer; entity must add its renderables to the list
    virtual void updateRenderList(RenderList& list) = 0;

    virtual bool isShadowCaster() const;
    virtual void setShadowCaster(bool shadows);
private:
    bool castShadows_;

    Entity(const Entity&);
    Entity& operator=(const Entity&);
};

}

#endif //ENTITY_H