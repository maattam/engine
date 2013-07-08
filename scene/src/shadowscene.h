#ifndef SHADOWSCENE_H
#define SHADOWSCENE_H

#include "abstractscene.h"
#include "light.h"
#include "camera.h"
#include "entity/mesh.h"

#include <QOpenGLFunctions_4_2_Core>

#include <memory>

class ShadowScene : public Engine::AbstractScene
{
public:
    ShadowScene();
    ~ShadowScene();

    void initialize(QOpenGLFunctions_4_2_Core* funcs);

    virtual Engine::Camera* activeCamera();
    virtual const Engine::DirectionalLight& queryDirectionalLight();
    virtual const std::vector<Engine::PointLight>& queryPointLights();
    virtual const std::vector<Engine::SpotLight>& querySpotLights();

    virtual void prepareScene(Engine::SceneNode* scene);

private:
    Engine::Camera camera_;

    std::shared_ptr<Engine::Mesh> sceneMesh_;

    std::vector<Engine::PointLight> pointLights_;
    std::vector<Engine::SpotLight> spotLights_;
    Engine::DirectionalLight directionalLight_;
};

#endif //SHADOWSCENE_H