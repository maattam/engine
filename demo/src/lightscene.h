#ifndef LIGHTSCENE_H
#define LIGHTSCENE_H

#include "freelookscene.h"

#include "scene/importednode.h"

#include <memory>

namespace Engine {
    class ResourceDespatcher;
}

class LightScene : public FreeLookScene
{
public:
    explicit LightScene(Engine::ResourceDespatcher& despatcher);
    ~LightScene();

    // Reimplemented methods from FreeLookScene
    virtual void update(unsigned int elapsed);

protected:
    // Implemented methods from FreeLookScene
    virtual void initialise();

private:
    Engine::ImportedNode::Ptr dragon_;

    struct PointLight
    {
        Engine::Graph::Light::Ptr light;

        float radius;
        float freq;
        float height;
        float phi;
        float hphi;
    };

    QVector<PointLight> lights_;
    double elapsed_;

    void insertLight(Engine::Graph::SceneNode* node);
};

#endif // LIGHTSCENE_H