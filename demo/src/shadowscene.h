#ifndef SHADOWSCENE_H
#define SHADOWSCENE_H

#include "abstractscene.h"
#include "entity/light.h"
#include "entity/camera.h"
#include "entity/mesh.h"
#include "colladanode.h"
#include "renderable/cube.h"
#include "cubemaptexture.h"

#include "common.h"

#include <memory>

namespace Engine {
    class ResourceDespatcher;
}

class ShadowScene : public Engine::AbstractScene
{
public:
    ShadowScene(Engine::ResourceDespatcher* despatcher);
    ~ShadowScene();

    virtual Engine::Entity::Camera* activeCamera();

    virtual void prepareScene();
    virtual void update(unsigned int elapsed);

    void release();

private:
    Engine::Entity::Camera camera_;
    Engine::ResourceDespatcher* despatcher_;

    Engine::ColladaNode::Ptr sceneMesh_;

    Engine::Entity::Mesh::Ptr sphere_;
    Engine::Graph::SceneNode* sphereNode_[2];

    Engine::Entity::Light directionalLight_;

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