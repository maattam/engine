// Node which holds Entities; forms the scene graph

#ifndef SCENENODE_H
#define SCENENODE_H

#include "node.h"

#include <QString>
#include <QList>

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

    // Attached entity to the node
    // precondition: entity != nullptr
    virtual void attachEntity(Entity::Entity* entity);

    // postcondition: entity detached if found, entity is not deleted
    virtual Entity::Entity* detachEntity(Entity::Entity* entity);
    virtual Entity::Entity* detachEntity(Entities::size_type index);
    virtual void detachAllEntities();

    Entities::size_type numEntities() const;

    // precondition: index < numEntities
    Entity::Entity* getEntity(Entities::size_type index);

    // Tells if the node's entities should cast shadows
    bool isShadowCaster() const;
    void setShadowCaster(bool shadows);

    // Returns a list of entities matching the given name.
    // The search is done recursively downwards the tree, so this function should
    // be called from the root node.
    QList<Entity::Entity*> findEntities(const QString& name) const;

    // Utility method for creating SceneNode children
    SceneNode* createSceneNodeChild();

protected:
    virtual Node* createChildImpl();

private:
    Entities entities_;
    bool castShadows_;
};

}}

#endif //SCENENODE_H