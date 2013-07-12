// Node which holds Entities
// Forms the scene graph

#ifndef SCENENODE_H
#define SCENENODE_H

#include "node.h"
#include "entity/entity.h"

namespace Engine {

class SceneNode : public Node
{
public:
    typedef std::vector<Entity*> Entities;

    SceneNode();
    virtual ~SceneNode();

    virtual void attachEntity(Entity* entity);

    virtual Entity* detachEntity(Entity* entity);
    virtual Entity* detachEntity(Entities::size_type index);
    virtual void detachAllEntities();

    virtual Entities::size_type numEntities() const;
    virtual Entity* getEntity(Entities::size_type index);

    virtual bool isShadowCaster() const;
    virtual void setShadowCaster(bool shadows);

protected:
    virtual Node* createChildImpl(const QMatrix4x4& transformation);

private:
    Entities entities_;
    bool castShadows_;
};

}

#endif //SCENENODE_H