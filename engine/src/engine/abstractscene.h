// Interface class for different level types

#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include "scenenode.h"

namespace Engine {

class Camera;
class Light;

class AbstractScene
{
public:
    AbstractScene();
    virtual ~AbstractScene();

    virtual Camera* activeCamera() = 0;
    virtual Light* activeLight() = 0;   // TODO

    virtual void prepareScene(SceneNode* root);

protected:
    SceneNode* sceneGraph();

private:
    SceneNode* scene_;

    AbstractScene(const AbstractScene&);
    AbstractScene& operator=(const AbstractScene&);
};

}

#endif //ABSTRACTSCENE_H