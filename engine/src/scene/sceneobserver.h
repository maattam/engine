//
//  Author   : Matti M‰‰tt‰
//  Summary  : SceneObserver receives events emitted by SceneManager. This allows extending
//             of SceneManager functionality.
//

#ifndef SCENEOBSERVER_H
#define SCENEOBSERVER_H

#include "observer.h"

namespace Engine {

namespace Graph {
    class SceneLeaf;
    class SceneNode;
}

class CubemapTexture;

class SceneObserver : public Observer<SceneObserver>
{
public:
    SceneObserver() {};
    virtual ~SceneObserver() {};

    // Called before the visible entity is pushed down the render queue.
    // If this function returns false, the entity is not pushed to the default render queue.
    virtual bool beforeRendering(Graph::SceneLeaf* entity, Graph::SceneNode* node) { return true; };

    // Called when the scene's skybox has been changed. The skybox can be nullptr.
    virtual void skyboxTextureUpdated(CubemapTexture* skybox) {};

    // Called when the scene has been modified so that the data from last prepareNextFrame should be discarded.
    virtual void sceneInvalidated() {};
};

}

#endif // SCENEOBSERVER_H