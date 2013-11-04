#include "sponzascene.h"

#include "graph/scenenode.h"
#include "resourcedespatcher.h"

#include <QDebug>
#include <qmath.h>

using namespace Engine;

SponzaScene::SponzaScene(ResourceDespatcher* despatcher) : FreeLookScene(despatcher),
    spotLight_(Entity::Light::LIGHT_SPOT), velocity_(10.0f), spotVelocity_(7.0f), elapsed_(0)
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

SponzaScene::~SponzaScene()
{
}

void SponzaScene::update(unsigned int elapsed)
{
    FreeLookScene::update(elapsed);

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
        spotLight_.setDirection(spotNode_->direction);
    }

    pos += spotDirection_ * t * spotVelocity_;
    sphereNode_[1]->setPosition(pos);
}

void SponzaScene::initialise()
{
    // Load skybox
    setSkyboxTexture("assets/skybox/miramar/miramar*.dds");

    // Load meshes
    sceneMesh_ = despatcher()->get<ColladaNode>("assets/sponza_scene.dae");
    sphere_ = despatcher()->get<Entity::Mesh>("assets/sphere.obj");

    // Set up lights
    setDirectionalLight(QVector3D(1, 1, 251 / 255.0f), QVector3D(0.0f, -1.0f, -0.09f), 0.01f, 4.5f);

    spotLight_.setColor(QVector3D(255, 214, 170) / 255.0f);
    spotLight_.setDirection(spotNode_->direction);
    spotLight_.setDiffuseIntensity(20.0f);
    spotLight_.setAttenuationExp(0.005f);
    spotLight_.setAttenuationConstant(1.0f);
    spotLight_.setAttenuationLinear(0.1f);
    spotLight_.setCutoff(30.0f);

    /*Entity::PointLight pointLight;
    pointLight.attenuation.exp = 0.005f;
    pointLight.attenuation.constant = 1.0f;
    pointLight.attenuation.linear = 0.2f;
    pointLight.position = QVector3D(0, 12, 0);
    pointLight.color = QVector3D(255, 241, 224) / 255.0f;
    pointLight.diffuseIntensity = 7.0f;
    pointLight.ambientIntensity = 0.5f;
    pointLights_.push_back(pointLight);*/

    // Position entities
    Graph::SceneNode* node = rootNode()->createSceneNodeChild();
    node->setScale(0.05f);
    sceneMesh_->attach(node);

    for(int i = 1; i < 2; ++i)
    {
        sphereNode_[i] = rootNode()->createSceneNodeChild();
        sphereNode_[i]->attachEntity(sphere_.get());
        sphereNode_[i]->setShadowCaster(false);
        sphereNode_[i]->setScale(0.5f);
    }

    sphereNode_[1]->attachEntity(&spotLight_);
    sphereNode_[1]->setPosition(spotPath_.back().endpoint);
}