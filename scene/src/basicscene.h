#ifndef BASICSCENE_H
#define BASICSCENE_H

#include "abstractscene.h"
#include "material.h"
#include "entity/light.h"
#include "entity/camera.h"
#include "entity/mesh.h"
#include "entity/boxprimitive.h"

#include "common.h"

#include <vector>
#include <memory>

class BasicScene : public Engine::AbstractScene
{
public:
    BasicScene();
    virtual ~BasicScene();

    void initialize(QOPENGL_FUNCTIONS* funcs);

    virtual Engine::Entity::Camera* activeCamera();
    virtual const Engine::Entity::DirectionalLight& queryDirectionalLight();
    virtual const std::vector<Engine::Entity::PointLight>& queryPointLights();
    virtual const std::vector<Engine::Entity::SpotLight>& querySpotLights();

    virtual void prepareScene(Engine::Graph::SceneNode* scene);
    virtual void update(unsigned int elapsed);

private:
    Engine::Entity::Camera camera_;

    std::vector<Engine::Entity::PointLight> pointLights_;
    std::vector<Engine::Entity::SpotLight> spotLights_;
    Engine::Entity::DirectionalLight directionalLight_;

    std::shared_ptr<Engine::Entity::Mesh> torus_;
    std::shared_ptr<Engine::Entity::Mesh> oildrum_;
    std::shared_ptr<Engine::Entity::Mesh> sphere_;
    std::shared_ptr<Engine::Entity::Mesh> platform_;
    std::shared_ptr<Engine::Entity::Mesh> hellknight_;

    std::shared_ptr<Engine::Entity::BoxPrimitive> cube_[2];

    Engine::Graph::SceneNode* cubeNode_;
    Engine::Graph::SceneNode* platformNode_;
    Engine::Graph::SceneNode* torusNode_;
    Engine::Graph::SceneNode* sphereNode_;

    std::vector<Engine::Graph::SceneNode*> cubes_;

    std::vector<Engine::Material::Ptr> materials_;
    std::vector<Engine::Entity::Light> lights_;

    unsigned int time_;

    QOPENGL_FUNCTIONS* gl;
};

#endif //BASICSCENE_H