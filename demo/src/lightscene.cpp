#include "lightscene.h"

#include "resourcedespatcher.h"
#include "graph/scenenode.h"
#include "scene/scenemanager.h"

#include <assimp/postprocess.h>

#include <QTime>
#include <qmath.h>

using namespace Engine;

LightScene::LightScene(ResourceDespatcher& despatcher)
    : FreeLookScene(despatcher), elapsed_(0)
{
    qsrand(QTime::currentTime().msec());
}

LightScene::~LightScene()
{
}

void LightScene::update(unsigned int elapsed)
{
    FreeLookScene::update(elapsed);

    elapsed_ += static_cast<float>(elapsed) / 1000;
    const float t = elapsed_ / 1000;

    // Update lights
    for(const PointLight& light : lights_)
    {
        QVector3D position;
        position.setX(light.radius * qSin(elapsed_ + light.phi));
        position.setY(light.height * (1 + qSin(elapsed_ * light.freq + light.hphi)));
        position.setZ(light.radius * qCos(elapsed_ + light.phi));

        light.light->parentNode()->setPosition(position);
    }
}

void LightScene::initialise()
{
    // Load skybox
    //setSkyboxTexture("assets/skybox/space/space*.png");
    setSkyboxTexture("assets/skybox/miramar/miramar*.dds");

    // Load meshes
    dragon_ = despatcher().get<ImportedNode>("assets/dragon.dae", scene(), aiProcessPreset_TargetRealtime_Fast);
    dragon_->attach(&rootNode());

    for(int i = 0; i < 500; ++i)
    {
        insertLight(&rootNode());
    }

    // Position the camera
    camera()->setPosition(QVector3D(-50, 15, 0));
}

void LightScene::insertLight(Engine::Graph::SceneNode* node)
{
    Graph::SceneNode* lightNode = node->createChild();
    PointLight light;

    light.light = createLight(Graph::Light::LIGHT_POINT);
    light.light->setColor(QVector3D(qrand() % 225 + 25, qrand() % 225 + 25, qrand() % 225 + 25) / 255.0f);
    light.light->setAttenuationQuadratic(1);
    light.light->setAttenuationLinear(0);

    light.radius = qrand() % 40 + 15;
    light.height = qrand() % 20 + 1;
    light.freq = qrand() % 5 + 1;
    light.phi = qDegreesToRadians(static_cast<float>(qrand() % 360));
    light.hphi = qDegreesToRadians(static_cast<float>(qrand() % 360));

    lights_.push_back(light);
    light.light->attach(lightNode);
}