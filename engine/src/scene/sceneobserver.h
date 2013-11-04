#ifndef SCENEOBSERVER_H
#define SCENEOBSERVER_H

#include <QMatrix4x4>

namespace Engine {

namespace Entity {
    class Entity;
}

namespace Graph {
    class SceneNode;
}

class SceneObserver
{
public:
    SceneObserver() {};
    virtual ~SceneObserver() {};

    // Called before the visible entity is pushed down the render queue.
    virtual void beforeRendering(Entity::Entity* entity, Graph::SceneNode* node) = 0;
};

}

#endif // SCENEOBSERVER_H