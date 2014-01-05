#ifndef BASICSCENE_H
#define BASICSCENE_H

#include "freelookscene.h"

#include "material.h"
#include "entity/mesh.h"
#include "entity/subentity.h"
#include "scene/importednode.h"

#include <QObject>

#include <vector>
#include <memory>

namespace Engine {
    class ResourceDespatcher;
}

class BasicScene : public QObject, public FreeLookScene
{
    Q_OBJECT

public:
    explicit BasicScene(Engine::ResourceDespatcher* despatcher);
    ~BasicScene();

    // Reimplemented methods from FreeLookScene
    virtual void update(unsigned int elapsed);

public slots:
    void resourceInitialized(const QString& name);

protected:
    // Implemented methods from FreeLookScene
    virtual void initialise();

private:
    Engine::ImportedNode::Ptr torus_;
    Engine::ImportedNode::Ptr oildrum_;
    Engine::ImportedNode::Ptr sphere_;
    Engine::ImportedNode::Ptr platform_;
    Engine::ImportedNode::Ptr hellknight_;

    std::shared_ptr<Engine::Entity::SubEntity> cube_[2];

    Engine::Graph::SceneNode* cubeNode_;
    Engine::Graph::SceneNode* platformNode_;
    Engine::Graph::SceneNode* hkNode_;
    Engine::Graph::SceneNode* torusNode_;
    Engine::Graph::SceneNode* sphereNode_;
    Engine::Graph::SceneNode* blueLightNode_;

    std::vector<Engine::Graph::SceneNode*> cubes_;
    std::vector<Engine::Entity::Light::Ptr> lights_;

    unsigned int time_;
};

#endif //BASICSCENE_H