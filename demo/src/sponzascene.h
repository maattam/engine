#ifndef SHADOWSCENE_H
#define SHADOWSCENE_H

#include "freelookscene.h"

#include "entity/mesh.h"
#include "scene/importednode.h"

#include <memory>

namespace Engine {
    class ResourceDespatcher;
}

class SponzaScene : public FreeLookScene
{
public:
    explicit SponzaScene(Engine::ResourceDespatcher* despatcher);
    ~SponzaScene();

    // Reimplemented methods from FreeLookScene
    virtual void update(unsigned int elapsed);

protected:
    // Implemented methods from FreeLookScene
    virtual void initialise();

private:
    Engine::ImportedNode::Ptr sceneMesh_;
    Engine::ImportedNode::Ptr sphere_;

    Engine::Graph::SceneNode* sphereNode_;
    Engine::Graph::SceneNode* cameraNode_;

    float velocity_;

    struct PathNode
    {
        QVector3D endpoint;
        QVector3D direction;
        PathNode* next;
    };

    std::vector<PathNode> spotPath_;
    QVector3D spotDirection_;
    PathNode* spotNode_;
    Engine::Entity::Light spotLight_;
    Engine::Entity::Light flashLight_;
    float spotVelocity_;
    bool flashLightToggle_;

    double elapsed_;
};

#endif //SHADOWSCENE_H