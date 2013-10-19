#ifndef BASICSCENE_H
#define BASICSCENE_H

#include "freelookscene.h"

#include "material.h"
#include "entity/mesh.h"
#include "entity/boxprimitive.h"
#include "scene/colladanode.h"

#include <vector>
#include <memory>

namespace Engine {
    class ResourceDespatcher;
}

class BasicScene : public FreeLookScene
{
public:
    explicit BasicScene(Engine::ResourceDespatcher* despatcher);
    ~BasicScene();

    // Reimplemented methods from FreeLookScene
    virtual void update(unsigned int elapsed);

protected:
    // Implemented methods from FreeLookScene
    virtual void initialise();

private:
    Engine::Entity::Mesh::Ptr torus_;
    Engine::Entity::Mesh::Ptr oildrum_;
    Engine::Entity::Mesh::Ptr sphere_;
    Engine::ColladaNode::Ptr platform_;
    Engine::Entity::Mesh::Ptr hellknight_;

    std::shared_ptr<Engine::Entity::BoxPrimitive> cube_[2];

    Engine::Graph::SceneNode* cubeNode_;
    Engine::Graph::SceneNode* platformNode_;
    Engine::Graph::SceneNode* torusNode_;
    Engine::Graph::SceneNode* sphereNode_;

    std::vector<Engine::Graph::SceneNode*> cubes_;
    std::vector<Engine::Entity::Light::Ptr> lights_;

    unsigned int time_;
};

#endif //BASICSCENE_H