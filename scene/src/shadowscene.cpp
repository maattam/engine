#include "shadowscene.h"

#include "graph/scenenode.h"

#include <QDebug>

using namespace Engine;

ShadowScene::ShadowScene()
    : camera_(QVector3D(-2, 6, 7), 0, 0.0f, 60.0f, 0.0f)
{
}

ShadowScene::~ShadowScene()
{
}

const Entity::DirectionalLight& ShadowScene::queryDirectionalLight()
{
    return directionalLight_;
}

const std::vector<Entity::PointLight>& ShadowScene::queryPointLights()
{
    return pointLights_;
}

const std::vector<Entity::SpotLight>& ShadowScene::querySpotLights()
{
    return spotLights_;
}

Entity::Camera* ShadowScene::activeCamera()
{
    // We only have a single camera
    return &camera_;
}

void ShadowScene::initialize(QOPENGL_FUNCTIONS* funcs)
{
    sceneMesh_ = std::make_shared<Entity::Mesh>(funcs);
    if(!sceneMesh_->loadFromFile("./assets/shadowtest.obj"))
    {
        qDebug() << "Failed to load Shadow!";
    }

    directionalLight_.diffuseIntensity = 1.0f;
    directionalLight_.color = QVector3D(0.7f, 0.3f, 0.1f);
    directionalLight_.direction = QVector3D(1.0f, -1.0f, -1.0f);

    Entity::SpotLight spotLight;
    spotLight.color = QVector3D(1.0f, 1.0f, 1.0f);
    spotLight.position = QVector3D(0.0f, 4.0f, 1.0f);
    spotLight.direction = QVector3D(0, 1, 0);
    spotLight.cutoff = 45.0f;
    spotLights_.push_back(spotLight);
}

void ShadowScene::prepareScene(Graph::SceneNode* scene)
{
    AbstractScene::prepareScene(scene);

    scene->attachEntity(sceneMesh_.get());
}