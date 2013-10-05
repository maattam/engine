#include "shadowscene.h"

#include "graph/scenenode.h"
#include "resourcedespatcher.h"

#include <QDebug>
#include <qmath.h>

using namespace Engine;

ShadowScene::ShadowScene(ResourceDespatcher* despatcher)
    : camera_(QVector3D(0, 10, 0), 1.5f, 0.0f, 75.0f, 0.0f, 400.0f),
    despatcher_(despatcher), velocity_(10.0f), spotVelocity_(7.0f), elapsed_(0)
{
    const int NUM_NODES = 4;
    const float h = 30.0f;
    const QVector3D positions[NUM_NODES] = { QVector3D(66, h, 29),
                                             QVector3D(-67, h, 29),
                                             QVector3D(-67, h, -28),
                                             QVector3D(66, h, -28)};

    const QVector3D directions[NUM_NODES] = {QVector3D(-1, 0, 0),
                                             QVector3D(0, 0, -1),
                                             QVector3D(1, 0, 0),
                                             QVector3D(0, 0, 1)};
    spotPath_.resize(NUM_NODES);

    // Construct path
    for(int i = 0; i < NUM_NODES; ++i)
    {
        PathNode& node = spotPath_[i];
        node.endpoint = positions[i];

        QVector3D dir = directions[i];
        dir.normalize();

        node.direction = dir;

        if(i > 0)
        {
            spotPath_[i-1].next = &node;
        }
    }

    // Connect loop
    spotPath_[NUM_NODES-1].next = &spotPath_[0];
    spotNode_ = &spotPath_[0];

    spotDirection_ = spotNode_->next->endpoint - spotNode_->endpoint;
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

CubemapTexture* ShadowScene::skyboxTexture()
{
    return skyboxTexture_.get();
}

Renderable::Renderable* ShadowScene::skyboxMesh()
{
    return &skyboxMesh_;
}

Entity::Camera* ShadowScene::activeCamera()
{
    // We only have a single camera
    return &camera_;
}

void ShadowScene::initialize()
{
    // Load skybox
    skyboxTexture_ = despatcher_->get<CubemapTexture>("assets/skybox/space*.png");
    skyboxTexture_->setFiltering(GL_LINEAR, GL_LINEAR);

    sceneMesh_ = despatcher_->get<ColladaNode>("assets/sponza_scene.dae");

    directionalLight_.color = QVector3D(201, 226, 255) / 255.0f * 0.1f;
    directionalLight_.direction = QVector3D(0.0f, -1.0f, 0.0f);

    Entity::SpotLight spotLight;
    spotLight.color = QVector3D(255, 214, 170) / 255.0f;
    spotLight.position = spotPath_.back().endpoint;
    spotLight.direction = spotNode_->direction;
    spotLight.diffuseIntensity = 10.0f;
    spotLight.attenuation.exp = 0.01f;
    spotLight.attenuation.constant = 1.0f;
    spotLight.attenuation.linear = 0.1f;
    spotLight.cutoff = 30.0f;
    spotLights_.push_back(spotLight);

    Entity::PointLight pointLight;
    pointLight.attenuation.exp = 0.05f;
    pointLight.attenuation.constant = 1.0f;
    pointLight.attenuation.linear = 0.2f;
    pointLight.position = QVector3D(0, 12, 0);
    pointLight.color = QVector3D(255, 241, 224) / 255.0f;
    pointLight.diffuseIntensity = 5.0f;
    pointLight.ambientIntensity = 0.5f;

    sphere_ = despatcher_->get<Entity::Mesh>("assets/sphere.obj");
    Engine::Material::Attributes attrib;
    attrib.diffuseColor = QVector3D(0.0f, 0.0f, 0.0f);
    attrib.ambientColor = 1.5f * pointLight.color;
    attrib.specularIntensity = 0.0f;
    sphere_->setMaterialAttributes(attrib);

    pointLights_.push_back(pointLight);
}

void ShadowScene::update(unsigned int elapsed)
{
    const float posMin = -65.0f;
    const float posMax = 65.0f;
    const float t = static_cast<float>(elapsed) / 1000;
    elapsed_ += t;

    // Bounce point light
    QVector3D pos = pointLights_[0].position;
    if(pos.x() > posMax || pos.x() < posMin)
    {
        if(pos.x() > posMax)
            pos.setX(posMax);

        else
            pos.setX(posMin);

        velocity_ = -velocity_;
    }

    pos.setX(pos.x() + t * velocity_);
    pointLights_[0].position = pos;
    sphereNode_[0]->setPosition(pos);

    // Flicker some
    /*QVector3D color(
        (1 + qCos(elapsed_ / M_PI))/2.0,
        (1 + qCos(elapsed_ / M_PI + M_PI/8.0))/2.0,
        (1 + qSin(elapsed_ / M_PI))/2.0
        );

    float intensity = 5 + 5000.0f / (pos - spotLights_[0].position).lengthSquared();

    pointLights_[0].color = color;
    pointLights_[0].diffuseIntensity = intensity;

    Engine::Material::Attributes attrib;
    attrib.diffuseColor = QVector3D(0.0f, 0.0f, 0.0f);
    attrib.ambientColor = intensity * color;
    attrib.specularIntensity = 0.0f;

    sphere_->setMaterialAttributes(attrib);*/

    // Traverse spot light path
    pos = spotLights_[0].position;
    if(QVector3D::dotProduct((pos - spotNode_->endpoint), spotDirection_) > 0)
    {
        pos = spotNode_->endpoint;
        spotDirection_ = spotNode_->next->endpoint - pos;
        spotDirection_.normalize();

        spotNode_ = spotNode_->next;
        spotLights_[0].direction = spotNode_->direction;
    }

    pos += spotDirection_ * t * spotVelocity_;
    spotLights_[0].position = pos;
    sphereNode_[1]->setPosition(pos);
}

void ShadowScene::prepareScene(Graph::SceneNode* scene)
{
    AbstractScene::prepareScene(scene);

    QMatrix4x4 scale;
    scale.scale(0.05f);

    Graph::SceneNode* node = dynamic_cast<Graph::SceneNode*>(scene->createChild());
    node->applyTransformation(scale);
    sceneMesh_->attach(node);

    scale.setToIdentity();
    scale.scale(0.5f);

    for(int i = 0; i < 2; ++i)
    {
        sphereNode_[i] = dynamic_cast<Graph::SceneNode*>(scene->createChild());
        sphereNode_[i]->attachEntity(sphere_.get());
        sphereNode_[i]->setShadowCaster(false);
        sphereNode_[i]->applyTransformation(scale);
    }
}