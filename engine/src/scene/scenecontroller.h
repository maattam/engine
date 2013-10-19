// Interface controller class for implementing scene logic.

#ifndef SCENECONTROLLER_H
#define SCENECONTROLLER_H

#include "scenemodel.h"

namespace Engine {

class SceneController
{
public:
    virtual ~SceneController() {};

    virtual void setModel(SceneModel* model) = 0;
    virtual void renderScene() = 0;
    virtual void update(unsigned int elapsed) = 0;
};

}

#endif // SCENECONTROLLER_H