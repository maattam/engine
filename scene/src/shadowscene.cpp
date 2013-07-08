#include "shadowscene.h"

#include <QDebug>

ShadowScene::ShadowScene()
    : camera_(QVector3D(-2, 6, 7), 0, 0.0f, 60.0f, 0.0f)
{
}

ShadowScene::~ShadowScene()
{
}

const Engine::DirectionalLight& ShadowScene::queryDirectionalLight()
{
    return directionalLight_;
}

const std::vector<Engine::PointLight>& ShadowScene::queryPointLights()
{
    return pointLights_;
}

const std::vector<Engine::SpotLight>& ShadowScene::querySpotLights()
{
    return spotLights_;
}

Engine::Camera* ShadowScene::activeCamera()
{
    // We only have a single camera
    return &camera_;
}

void ShadowScene::initialize(QOpenGLFunctions_4_2_Core* funcs)
{
    sceneMesh_ = std::make_shared<Engine::Mesh>(funcs);
    if(!sceneMesh_->loadFromFile("./assets/shadowtest.obj"))
    {
        qDebug() << "Failed to load Shadow!";
    }

    directionalLight_.diffuseIntensity = 1.0f;
    directionalLight_.color = QVector3D(0.7f, 0.3f, 0.1f);
    directionalLight_.direction = QVector3D(1.0f, -1.0f, -1.0f);

    Engine::SpotLight spotLight;
    spotLight.color = QVector3D(1.0f, 1.0f, 1.0f);
    spotLight.position = QVector3D(0.0f, 4.0f, 1.0f);
    spotLight.direction = QVector3D(0, 1, 0);
    spotLight.cutoff = 45.0f;
    spotLights_.push_back(spotLight);
}

void ShadowScene::prepareScene(Engine::SceneNode* scene)
{
    AbstractScene::prepareScene(scene);

    scene->attachEntity(sceneMesh_.get());
}