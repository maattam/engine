// Interface model class for deriving different scene architectures.

#ifndef SCENEMODEL_H
#define SCENEMODEL_H

#include "cubemaptexture.h"
#include "sceneobserver.h"
#include "observable.h"

namespace Engine {

class Renderer;

namespace Entity {
    class Camera;
    class Light;
}

namespace Graph {
    class SceneNode;
}

class SceneModel : public Observable<SceneObserver>
{
public:
    virtual ~SceneModel() {};

    virtual void setView(Renderer* view) = 0;
    virtual void renderScene(Entity::Camera* camera) = 0;

    virtual void setDirectionalLight(Entity::Light* light) = 0;
    virtual void setSkybox(const CubemapTexture::Ptr& texture) = 0;

    // Returns the root node of the scene hierarchy
    virtual Graph::SceneNode* rootNode() = 0;
};

}

#endif // SCENEMODEL_H