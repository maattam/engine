#ifndef BASICSCENE_H
#define BASICSCENE_H

#include "engine/abstractscene.h"
#include "engine/material.h"
#include "engine/light.h"
#include "engine/camera.h"
#include "engine/mesh.h"
#include "engine/boxprimitive.h"

#include <QOpenGLFunctions_4_2_Core>

#include <vector>
#include <memory>

class BasicScene : public Engine::AbstractScene
{
public:
    explicit BasicScene();
    virtual ~BasicScene();

    void initialize(QOpenGLFunctions_4_2_Core* funcs);
    void update(float elapsed);

    virtual Engine::Camera* activeCamera();
    virtual Engine::Light* activeLight();

    virtual void prepareScene(Engine::SceneNode* scene);

private:
    Engine::Camera camera_;

    std::shared_ptr<Engine::Mesh> torus_;
    std::shared_ptr<Engine::Mesh> oildrum_;
    std::shared_ptr<Engine::Mesh> sphere_;
    std::shared_ptr<Engine::Mesh> suzanne_;

    std::shared_ptr<Engine::BoxPrimitive> cube_[2];

    Engine::SceneNode* oildrumNode_;
    Engine::SceneNode* cubeNode_;
    Engine::SceneNode* monkeyNode_;
    Engine::SceneNode* torusNode_;

    std::vector<Engine::SceneNode*> cubes_;

    std::vector<Engine::Material::Ptr> materials_;
    std::vector<Engine::Light> lights_;

    QOpenGLFunctions_4_2_Core* gl;
};

#endif //BASICSCENE_H