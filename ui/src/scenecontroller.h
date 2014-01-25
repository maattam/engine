//
//  Author   : Matti M‰‰tt‰
//  Summary  : Abstract scene representation.
// 

#ifndef SCENECONTROLLER_H
#define SCENECONTROLLER_H

namespace Engine { 

class SceneManager;
class ResourceDespatcher;

namespace Ui {

class InputState;

class SceneController
{
public:
    explicit SceneController(ResourceDespatcher& despatcher) {};
    virtual ~SceneController() {};

    virtual void setInput(InputState* input) = 0;

    // Camera parameters
    virtual void setFov(float angle) = 0;

    virtual void setManager(SceneManager* manager) = 0;
    virtual void update(unsigned int elapsed) = 0;
};

}};

#endif // SCENECONTROLLER_H