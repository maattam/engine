// Node which holds Entities
// Forms the scene graph

#ifndef SCENENODE_H
#define SCENENODE_H

#include "node.h"
#include "entity/aabb.h"

namespace Engine {
    
    namespace Entity {
        class Entity;
    }
    
namespace Graph {

class SceneNode : public Node
{
public:
    typedef std::vector<Entity::Entity*> Entities;

    SceneNode();
    virtual ~SceneNode();

    virtual void attachEntity(Entity::Entity* entity);

    virtual Entity::Entity* detachEntity(Entity::Entity* entity);
    virtual Entity::Entity* detachEntity(Entities::size_type index);
    virtual void detachAllEntities();

    Entities::size_type numEntities() const;
    Entity::Entity* getEntity(Entities::size_type index);

    bool isShadowCaster() const;
    void setShadowCaster(bool shadows);

    //const AABB& boundingBox();

protected:
    virtual Node* createChildImpl(const QMatrix4x4& transformation);
    //virtual void updateBounds(Node* node);

private:
    Entities entities_;
    bool castShadows_;
    //AABB boundingBox_;
};

}}

#endif //SCENENODE_H