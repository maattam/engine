#ifndef SHADOWSCENE_H
#define SHADOWSCENE_H

#include "abstractscene.h"
#include "entity/light.h"
#include "entity/camera.h"
#include "entity/mesh.h"
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

    void initialize();

    virtual Engine::Entity::Camera* activeCamera();
    virtual const Engine::Entity::DirectionalLight& queryDirectionalLight();
    virtual const std::vector<Engine::Entity::PointLight>& queryPointLights();
    virtual const std::vector<Engine::Entity::SpotLight>& querySpotLights();

    virtual Engine::CubemapTexture* skyboxTexture();
    virtual Engine::Renderable::Renderable* skyboxMesh();

    virtual void prepareScene(Engine::Graph::SceneNode* scene);
    virtual void update(unsigned int elapsed);

    void release();

private:
    Engine::Entity::Camera camera_;
    Engine::ResourceDespatcher* despatcher_;

    Engine::Entity::Mesh::Ptr sceneMesh_;
    Engine::Entity::Mesh::Ptr sphere_;
    Engine::Graph::SceneNode* sphereNode_[2];

    Engine::Renderable::Cube skyboxMesh_;
    std::shared_ptr<Engine::CubemapTexture> skyboxTexture_;

    std::vector<Engine::Entity::PointLight> pointLights_;
    std::vector<Engine::Entity::SpotLight> spotLights_;
    Engine::Entity::DirectionalLight directionalLight_;

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
    float spotVelocity_;

    double elapsed_;
};

#endif //SHADOWSCENE_H