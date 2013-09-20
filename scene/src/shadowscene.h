#ifndef SHADOWSCENE_H
#define SHADOWSCENE_H

#include "abstractscene.h"
#include "entity/light.h"
#include "entity/camera.h"
#include "entity/mesh.h"

#include "common.h"

#include <memory>

class ShadowScene : public Engine::AbstractScene
{
public:
    ShadowScene();
    ~ShadowScene();

    void initialize(QOPENGL_FUNCTIONS* funcs);

    virtual Engine::Entity::Camera* activeCamera();
    virtual const Engine::Entity::DirectionalLight& queryDirectionalLight();
    virtual const std::vector<Engine::Entity::PointLight>& queryPointLights();
    virtual const std::vector<Engine::Entity::SpotLight>& querySpotLights();

    virtual void prepareScene(Engine::Graph::SceneNode* scene);

private:
    Engine::Entity::Camera camera_;

    std::shared_ptr<Engine::Entity::Mesh> sceneMesh_;

    std::vector<Engine::Entity::PointLight> pointLights_;
    std::vector<Engine::Entity::SpotLight> spotLights_;
    Engine::Entity::DirectionalLight directionalLight_;
};

#endif //SHADOWSCENE_H