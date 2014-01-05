// Node which holds Entities; forms the scene graph

#ifndef SCENENODE_H
#define SCENENODE_H

#include "node.h"

#include <QString>
#include <QList>

namespace Engine {
    
namespace Graph {

class SceneLeaf;

class SceneNode : public Node
{
public:
    typedef std::vector<SceneLeaf*> Entities;

    SceneNode();
    virtual ~SceneNode();

    // Attached entity to the node
    // precondition: entity != nullptr
    virtual void attachEntity(SceneLeaf* entity);

    // postcondition: entity detached if found, entity is not deleted
    virtual SceneLeaf* detachEntity(SceneLeaf* entity);
    virtual SceneLeaf* detachEntity(Entities::size_type index);
    virtual void detachAllEntities();

    Entities::size_type numEntities() const;

    // precondition: index < numEntities
    SceneLeaf* getEntity(Entities::size_type index);

    // Tells if the node's entities should cast shadows
    bool isShadowCaster() const;
    void setShadowCaster(bool shadows);

    // Returns a list of entities matching the given name.
    // Multiple entities of different type can share the same name when they belong to the same node.
    // The search is done recursively downwards the tree, so this function should
    // be called from the root node.
    QList<SceneLeaf*> findEntities(const QString& name) const;

    // Convenienve funtions that filters findEntities to match the template parameter.
    template<typename EntityType>
    EntityType* findEntity(const QString& name) const;

    // Utility method for creating SceneNode children
    SceneNode* createSceneNodeChild();

protected:
    virtual Node* createChildImpl();

private:
    Entities entities_;
    bool castShadows_;
};

#include "scenenode.inl"

}}

#endif //SCENENODE_H