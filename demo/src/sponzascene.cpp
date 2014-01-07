#include "sponzascene.h"

#include "graph/scenenode.h"
#include "resourcedespatcher.h"
#include "inputstate.h"
#include "scene/scenemanager.h"

#include <QDebug>
#include <QTime>
#include <qmath.h>

using namespace Engine;

SponzaScene::SponzaScene(ResourceDespatcher& despatcher)
    : FreeLookScene(despatcher),
    velocity_(10.0f), spotVelocity_(7.0f), elapsed_(0), flashLightToggle_(false)
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

    qsrand(QTime::currentTime().msec());
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
    QVector3D pos = sphereNode_->position();
    if(QVector3D::dotProduct((pos - spotNode_->endpoint), spotDirection_) > 0)
    {
        pos = spotNode_->endpoint;
        spotDirection_ = spotNode_->next->endpoint - pos;
        spotDirection_.normalize();

        spotNode_ = spotNode_->next;
        spotLight_->setDirection(spotNode_->direction);
    }

    pos += spotDirection_ * t * spotVelocity_;
    sphereNode_->setPosition(pos);

    if(input()->keyDown(Qt::Key::Key_F))
    {
        input()->setKey(Qt::Key::Key_F, false);
        flashLightToggle_ = !flashLightToggle_;

        if(flashLightToggle_)
        {
            flashLight_->attach(cameraNode_);
        }

        else
        {
            flashLight_->detach();
        }
    }

    if(flashLightToggle_)
    {
        const QVector3D flashPos = camera()->position() - QVector3D(0, 2, 0);

        cameraNode_->setPosition(flashPos);
        flashLight_->setDirection(camera()->direction());
    }
}

void SponzaScene::initialise()
{
    // Load skybox
    setSkyboxTexture("assets/skybox/miramar/miramar*.dds");

    // Load meshes
    sceneMesh_ = despatcher().get<ImportedNode>("assets/sponza_scene.dae", scene());
    sphere_ = despatcher().get<ImportedNode>("assets/sphere.obj", scene());

    // Set up lights
    //setDirectionalLight(QVector3D(1, 1, 251 / 255.0f), QVector3D(0.0f, -1.0f, -0.09f), 0.05f, 5.0f);

    spotLight_ = createLight(Graph::Light::LIGHT_SPOT);
    spotLight_->setColor(QVector3D(255, 214, 170) / 255.0f);
    spotLight_->setDirection(spotNode_->direction);
    spotLight_->setDiffuseIntensity(40.0f);
    spotLight_->setAttenuationQuadratic(0.05f);
    spotLight_->setAttenuationConstant(1.0f);
    spotLight_->setAttenuationLinear(0.1f);
    spotLight_->setAngleOuterCone(30.0f);
    spotLight_->setLightMask(Graph::Light::MASK_CAST_SHADOWS);

    flashLight_ = createLight(Graph::Light::LIGHT_SPOT);
    flashLight_->setColor(QVector3D(1, 1, 1));
    flashLight_->setDiffuseIntensity(25.0f);
    flashLight_->setAttenuationQuadratic(0.2f);
    flashLight_->setAngleOuterCone(30.0f);
    flashLight_->setLightMask(Graph::Light::MASK_CAST_SHADOWS);
    flashLight_->detach();

    // Position entities
    Graph::SceneNode* node = rootNode().createChild();
    node->setScale(0.05f);
    sceneMesh_->attach(node);

    sphereNode_ = rootNode().createChild();
    sphere_->attach(sphereNode_);

    sphereNode_->setScale(0.5f);
    sphereNode_->setPosition(spotPath_.back().endpoint);
    spotLight_->attach(sphereNode_);

    cameraNode_ = rootNode().createChild();
    camera()->attach(cameraNode_);

    // Add bunch of lights
    for(int j = 0; j < 3; ++j)
    {
        for(int i = 0; i < 10; ++i)
        {
            Graph::Light::Ptr light = createLight(Graph::Light::LIGHT_POINT);
            light->setColor(QVector3D(qrand() % 225 + 25, qrand() % 225 + 25, qrand() % 225 + 25) / 255.0f);
            light->setDiffuseIntensity(25.0f);

            Graph::SceneNode* lightNode = rootNode().createChild();
            lightNode->setPosition(QVector3D(-50 + i * 10, 3, -20 + j * 20));

            light->attach(lightNode);
        }
    }

    //setFlySpeed(200.0f);
}