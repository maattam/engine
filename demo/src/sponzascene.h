//
//  Author   : Matti Määttä
//  Summary  : 
//

#ifndef SPONZASCENE_H
#define SPONZASCENE_H

#include "freelookscene.h"

#include "scene/importednode.h"
#include "graph/geometry.h"

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
    struct PathNode
    {
        QVector3D endpoint;
        QVector3D direction;
        PathNode* next;
    };

    struct Projectile
    {
        float velocity;
        QVector3D direction;
        Engine::Graph::Light::Ptr light;
        Engine::Graph::Geometry::Ptr object;
    };

    Engine::ImportedNode::Ptr sceneMesh_;
    Engine::ImportedNode::Ptr sphere_;

    Engine::Graph::SceneNode* sphereNode_;
    Engine::Graph::SceneNode* cameraNode_;

    float velocity_;

    std::vector<PathNode> spotPath_;
    QVector3D spotDirection_;
    PathNode* spotNode_;

    Engine::Graph::Light::Ptr spotLight_;
    Engine::Graph::Light::Ptr flashLight_;
    float spotVelocity_;
    bool flashLightToggle_;

    double elapsed_;

    std::vector<Projectile> projectiles_;

    Engine::Graph::Light::Ptr insertLight(Engine::Graph::SceneNode* node);

    void addProjectile(Projectile&& projectile, const QVector3D& initialPosition);
    void updateProjectiles(float elapsed);
};

#endif // SPONZASCENE_H