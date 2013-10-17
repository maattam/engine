// Interface class for different level types

#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include "scene.h"

#include <vector>

namespace Engine {

namespace Entity {
    class Camera;
}

class AbstractScene
{
public:
    AbstractScene();
    virtual ~AbstractScene();

    virtual Entity::Camera* activeCamera() = 0;

    virtual void prepareScene() = 0;
    virtual void update(unsigned int elapsed);

    Scene* scene();

private:
    Scene scene_;

    AbstractScene(const AbstractScene&);
    AbstractScene& operator=(const AbstractScene&);
};

}

#endif //ABSTRACTSCENE_H