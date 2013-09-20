// Interface class for different level types

#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include "entity/light.h"

#include <vector>

namespace Engine {

namespace Entity {
    class Camera;
}

namespace Graph {
    class SceneNode;
}

class AbstractScene
{
public:
    AbstractScene();
    virtual ~AbstractScene();

    virtual Entity::Camera* activeCamera() = 0;
    virtual const Entity::DirectionalLight& queryDirectionalLight() = 0;
    virtual const std::vector<Entity::PointLight>& queryPointLights() = 0;
    virtual const std::vector<Entity::SpotLight>& querySpotLights() = 0;

    virtual void prepareScene(Graph::SceneNode* root);
    virtual void update(unsigned int elapsed);

protected:
    Graph::SceneNode* sceneGraph();

private:
    Graph::SceneNode* scene_;

    AbstractScene(const AbstractScene&);
    AbstractScene& operator=(const AbstractScene&);
};

}

#endif //ABSTRACTSCENE_H