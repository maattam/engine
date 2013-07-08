// Interface class for different level types

#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include "scenenode.h"
#include "light.h"

namespace Engine {

class Camera;
struct Light;

class AbstractScene
{
public:
    AbstractScene();
    virtual ~AbstractScene();

    virtual Camera* activeCamera() = 0;
    virtual const DirectionalLight& queryDirectionalLight() = 0;
    virtual const std::vector<PointLight>& queryPointLights() = 0;
    virtual const std::vector<SpotLight>& querySpotLights() = 0;

    virtual void prepareScene(SceneNode* root);
    virtual void update(unsigned int elapsed);

protected:
    SceneNode* sceneGraph();

private:
    SceneNode* scene_;

    AbstractScene(const AbstractScene&);
    AbstractScene& operator=(const AbstractScene&);
};

}

#endif //ABSTRACTSCENE_H