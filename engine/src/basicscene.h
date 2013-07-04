#ifndef BASICSCENE_H
#define BASICSCENE_H

#include "engine/abstractscene.h"
#include "engine/texture.h"
#include "engine/light.h"
#include "engine/camera.h"
#include "engine/mesh.h"

#include <QOpenGLFunctions_4_2_Core>

#include <vector>

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

    Engine::Mesh* torus_;
    Engine::Mesh* oildrum_;
    Engine::Mesh* sphere_;
    Engine::Mesh* suzanne_;

    Engine::SceneNode* oildrumNode_;
    Engine::SceneNode* cubeNode_;
    Engine::SceneNode* monkeyNode_;
    Engine::SceneNode* torusNode_;

    std::vector<Engine::Renderable*> renderables_;
    std::vector<Engine::Texture*> textures_;
    std::vector<Engine::Light> lights_;

    QOpenGLFunctions_4_2_Core* gl;
};

#endif //BASICSCENE_H