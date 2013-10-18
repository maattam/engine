#include "shadowscene.h"

#include "graph/scenenode.h"
#include "resourcedespatcher.h"

#include <QDebug>
#include <qmath.h>

using namespace Engine;

ShadowScene::ShadowScene(ResourceDespatcher* despatcher)
    : camera_(Entity::Camera::PERSPECTIVE, QVector3D(-1, 0, 0)), spotLight_(Entity::Light::LIGHT_SPOT),
    despatcher_(despatcher), velocity_(10.0f), spotVelocity_(7.0f), elapsed_(0),
    directionalLight_(Entity::Light::LIGHT_DIRECTIONAL)
{
    camera_.setFov(75.0f);
    camera_.setFarPlane(400.0f);
    camera_.setPosition(QVector3D(0, 10, 0));

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

Entity::Camera* ShadowScene::activeCamera()
{
    // We only have a single camera
    return &camera_;
}

void ShadowScene::update(unsigned int elapsed)
{
    const float posMin = -65.0f;
    const float posMax = 65.0f;
    const float t = static_cast<float>(elapsed) / 1000;
    elapsed_ += t;

    // Traverse spot light path
    QVector3D pos = sphereNode_[1]->position();
    if(QVector3D::dotProduct((pos - spotNode_->endpoint), spotDirection_) > 0)
    {
        pos = spotNode_->endpoint;
        spotDirection_ = spotNode_->next->endpoint - pos;
        spotDirection_.normalize();

        spotNode_ = spotNode_->next;
        spotLight_.direction = spotNode_->direction;
    }

    pos += spotDirection_ * t * spotVelocity_;
    sphereNode_[1]->setPosition(pos);
}

void ShadowScene::prepareScene()
{
    // Load skybox
    CubemapTexture::Ptr skybox = despatcher_->get<CubemapTexture>("assets/skybox/miramar/miramar*.dds");
    skybox->setFiltering(GL_LINEAR, GL_LINEAR);
    scene()->setSkybox(skybox);

    sceneMesh_ = despatcher_->get<ColladaNode>("assets/sponza_scene.dae");

    directionalLight_.color = QVector3D(1, 1, 1) * 2.0f;
    directionalLight_.direction = QVector3D(0.0f, -1.0f, -0.09f);
    directionalLight_.ambientIntensity = 0.01f;
    scene()->setDirectionalLight(&directionalLight_);

    spotLight_.color = QVector3D(255, 214, 170) / 255.0f * 1.5;
    spotLight_.direction = spotNode_->direction;
    spotLight_.diffuseIntensity = 10.0f;
    spotLight_.attenuation.exp = 0.005f;
    spotLight_.attenuation.constant = 1.0f;
    spotLight_.attenuation.linear = 0.1f;
    spotLight_.cutoff = 30.0f;

    /*Entity::PointLight pointLight;
    pointLight.attenuation.exp = 0.005f;
    pointLight.attenuation.constant = 1.0f;
    pointLight.attenuation.linear = 0.2f;
    pointLight.position = QVector3D(0, 12, 0);
    pointLight.color = QVector3D(255, 241, 224) / 255.0f;
    pointLight.diffuseIntensity = 7.0f;
    pointLight.ambientIntensity = 0.5f;
    pointLights_.push_back(pointLight);*/

    sphere_ = despatcher_->get<Entity::Mesh>("assets/sphere.obj");

    QMatrix4x4 scale;
    scale.scale(0.05f);

    Graph::SceneNode* node = dynamic_cast<Graph::SceneNode*>(scene()->rootNode()->createChild());
    node->applyTransformation(scale);
    sceneMesh_->attach(node);

    scale.setToIdentity();
    scale.scale(0.5f);

    for(int i = 1; i < 2; ++i)
    {
        sphereNode_[i] = dynamic_cast<Graph::SceneNode*>(scene()->rootNode()->createChild());
        sphereNode_[i]->attachEntity(sphere_.get());
        sphereNode_[i]->setShadowCaster(false);
        sphereNode_[i]->applyTransformation(scale);
    }

    sphereNode_[1]->attachEntity(&spotLight_);
    sphereNode_[1]->setPosition(spotPath_.back().endpoint);
}