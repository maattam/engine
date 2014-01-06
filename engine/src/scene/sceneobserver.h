#ifndef SCENEOBSERVER_H
#define SCENEOBSERVER_H

#include <QMatrix4x4>

namespace Engine {

namespace Graph {
    class SceneLeaf;
    class SceneNode;
}

class CubemapTexture;

class SceneObserver
{
public:
    SceneObserver() {};
    virtual ~SceneObserver() {};

    // Called before the visible entity is pushed down the render queue.
    // If this function returns false, the entity is not pushed to the default render queue.
    virtual bool beforeRendering(Graph::SceneLeaf* entity, Graph::SceneNode* node) = 0;
};

}

#endif // SCENEOBSERVER_H