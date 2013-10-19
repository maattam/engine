#ifndef SHADOWSCENE_H
#define SHADOWSCENE_H

#include "freelookscene.h"

#include "entity/mesh.h"
#include "scene/colladanode.h"

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
    Engine::ColladaNode::Ptr sceneMesh_;

    Engine::Entity::Mesh::Ptr sphere_;
    Engine::Graph::SceneNode* sphereNode_[2];

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
    float spotVelocity_;

    double elapsed_;
};

#endif //SHADOWSCENE_H