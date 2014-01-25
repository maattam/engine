//
//  Author   : Matti Määttä
//  Summary  : 
//

#ifndef SPONZASCENE_H
#define SPONZASCENE_H

#include "freelookscene.h"

#include "scene/importednode.h"

namespace Engine {
    class ResourceDespatcher;
}

class SponzaScene : public Engine::Ui::FreeLookScene
{
public:
    explicit SponzaScene(Engine::ResourceDespatcher& despatcher);
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
    Engine::Graph::Light::Ptr spotLight_;
    Engine::Graph::Light::Ptr flashLight_;
    float spotVelocity_;
    bool flashLightToggle_;

    double elapsed_;
};

#endif // SPONZASCENE_H