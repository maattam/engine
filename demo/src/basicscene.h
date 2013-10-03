#ifndef BASICSCENE_H
#define BASICSCENE_H

#include "abstractscene.h"
#include "material.h"
#include "entity/light.h"
#include "entity/camera.h"
#include "entity/mesh.h"
#include "entity/boxprimitive.h"
#include "renderable/cube.h"
#include "colladanode.h"

#include <vector>
#include <memory>

namespace Engine {
    class ResourceDespatcher;
}

class BasicScene : public Engine::AbstractScene
{
public:
    BasicScene(Engine::ResourceDespatcher* despatcher);
    virtual ~BasicScene();

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
    Engine::ResourceDespatcher* despatcher_;

    Engine::Entity::Camera camera_;

    std::vector<Engine::Entity::PointLight> pointLights_;
    std::vector<Engine::Entity::SpotLight> spotLights_;
    Engine::Entity::DirectionalLight directionalLight_;

    Engine::Entity::Mesh::Ptr torus_;
    Engine::Entity::Mesh::Ptr oildrum_;
    Engine::Entity::Mesh::Ptr sphere_;
    Engine::ColladaNode::Ptr platform_;
    Engine::Entity::Mesh::Ptr hellknight_;

    std::shared_ptr<Engine::CubemapTexture> skyboxTexture_;
    Engine::Renderable::Cube skyboxMesh_;

    std::shared_ptr<Engine::Entity::BoxPrimitive> cube_[2];

    Engine::Graph::SceneNode* cubeNode_;
    Engine::Graph::SceneNode* platformNode_;
    Engine::Graph::SceneNode* torusNode_;
    Engine::Graph::SceneNode* sphereNode_;

    std::vector<Engine::Graph::SceneNode*> cubes_;
    std::vector<Engine::Entity::Light> lights_;

    unsigned int time_;
};

#endif //BASICSCENE_H