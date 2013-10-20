#include "basicscene.h"

#include "graph/scenenode.h"
#include "resourcedespatcher.h"

#include <qmath.h>
#include <QDebug>

using namespace Engine;

BasicScene::BasicScene(ResourceDespatcher* despatcher)
    : FreeLookScene(despatcher), time_(0)
{
}

BasicScene::~BasicScene()
{
}

void BasicScene::update(unsigned int elapsedMs)
{
    FreeLookScene::update(elapsedMs);

    const float R = 16.0f;
    const float R2 = 75.0f;

    float elapsed = static_cast<float>(elapsedMs) / 1000;
    float angle = static_cast<float>(time_) / 1000;

    time_ += elapsedMs;

    blueLightNode_->setPosition(QVector3D(R2 * sinf(angle), R2/2 * sinf(angle * 3), R2 * cosf(angle)));
    sphereNode_->setPosition(QVector3D(R/2 * sinf(angle * 3), R * sinf(angle), R * cosf(angle)));

    torusNode_->rotate(15.0f * elapsed, QVector3D(1, 1, 0));
    platformNode_->rotate(3.0f * elapsed, UNIT_Y);

    for(size_t i = 0; i < cubeNode_->numChildren(); ++i)
    {
        if(i % 2 == 0)
        {
            cubeNode_->getChild(i)->rotate(-5.0f * elapsed, QVector3D(0, 1, 0));
        }
    }

    cubeNode_->rotate(-3.0f * elapsed, UNIT_Y);

    for(size_t i = 0; i < cubes_.size(); ++i)
    {
        if(i % 2 == 0)
            cubes_[i]->rotate(15.0f * elapsed, QVector3D(1, 1, 0));

        else
            cubes_[i]->rotate(15.0f * elapsed, QVector3D(1, 0, 0));
    }

    //hkNode_->lookAt(playerPosition());
}

void BasicScene::initialise()
{
    // Load skybox
    setSkyboxTexture("assets/skybox/space/space*.png");

    // Set up directional light
    setDirectionalLight(QVector3D(1, 1, 1), QVector3D(1.0f, -1.0f, -1.0f), 0.0f, 0.05f);

    // Set up point lights
    Entity::Light::Ptr pointLight = std::make_shared<Entity::Light>(Entity::Light::LIGHT_POINT);
    pointLight->diffuseIntensity = 10.0f;
    pointLight->attenuation.exp = 0.1f;
    lights_.push_back(pointLight);

    pointLight = std::make_shared<Entity::Light>(Entity::Light::LIGHT_POINT);
    //pointLight.diffuseIntensity = 50.0f;
    pointLight->color = QVector3D(0.0f, 0.0f, 1.0f);
    pointLight->attenuation.exp = 0.025f;
    lights_.push_back(pointLight);

    blueLightNode_ = dynamic_cast<Graph::SceneNode*>(rootNode()->createChild());
    blueLightNode_->attachEntity(pointLight.get());

    // Set up spot lights
    Entity::Light::Ptr spotLight = std::make_shared<Entity::Light>(Entity::Light::LIGHT_SPOT);
    spotLight->color = QVector3D(1.0f, 0.0f, 1.0f);
    //spotLight->position = 2*QVector3D(-6.0f, 7/2, 6.0f);
    spotLight->direction = QVector3D(4.0f, -4.0f, -6.0f);
    spotLight->diffuseIntensity = 20.0f;
    spotLight->attenuation.exp = 0.05f;
    spotLight->cutoff = 20.0f;
    lights_.push_back(spotLight);

    // Load models
    oildrum_ = despatcher()->get<Entity::Mesh>("assets/oildrum.dae");
    hellknight_ = despatcher()->get<Entity::Mesh>("assets/hellknight/hellknight.md5mesh");
    platform_ = despatcher()->get<ColladaNode>("assets/blocks.dae");
    sphere_ = despatcher()->get<Entity::Mesh>("assets/sphere.obj");
    torus_ = despatcher()->get<Entity::Mesh>("assets/torus.obj");

    // Load cubes
    for(int i = 0; i < 2; ++i)
    {
        QString file = "assets/wooden_crate" + QString::number(i+1) + ".png";

        Texture2D::Ptr tex = despatcher()->get<Texture2D>(file);
        if(tex != nullptr)
        {
            Engine::Material::Ptr mat = std::make_shared<Engine::Material>(despatcher());
            mat->setTexture(Engine::Material::TEXTURE_DIFFUSE, tex);

            cube_[i] = std::make_shared<Entity::BoxPrimitive>();
            cube_[i]->setMaterial(mat);
        }
    }

    Graph::SceneNode* root = rootNode();

    // Create nodes
    platformNode_ = root->createSceneNodeChild();
    torusNode_ = root->createSceneNodeChild();
    cubeNode_ = root->createSceneNodeChild();
    sphereNode_ = root->createSceneNodeChild();

    // Oildrum
    {
        // Create rotation relation
        Graph::SceneNode* platf = platformNode_->createSceneNodeChild();
        platf->rotate(-90.0f, UNIT_X);

        Graph::SceneNode* node = platf->createSceneNodeChild();

        node->setPosition(QVector3D(-5, -2, 0));
        node->attachEntity(oildrum_.get());

        node = platf->createSceneNodeChild();

        node->setPosition(QVector3D(2, -2, 0));
        node->attachEntity(oildrum_.get());

        hkNode_ = platf->createSceneNodeChild();

        hkNode_->setPosition(QVector3D(-2, 2, 0));
        hkNode_->setScale(0.025f);

        hkNode_->attachEntity(hellknight_.get());
    }

    // Torus
    {
        torusNode_->setScale(180.0f);
        torusNode_->attachEntity(torus_.get());
        torusNode_->setShadowCaster(false);
    }

    // Sphere
    {
        sphereNode_->setScale(0.2f);
        sphereNode_->attachEntity(sphere_.get());
        sphereNode_->setShadowCaster(false);
        sphereNode_->attachEntity(lights_[0].get());

        // Platform spot light
        Graph::SceneNode* node = root->createSceneNodeChild();
        node->setPosition(2*QVector3D(-6.0f, 7/2, 6.0f));
        node->setScale(0.1f);
        node->attachEntity(sphere_.get());
        node->setShadowCaster(false);
        node->attachEntity(spotLight.get());
    }

    // platform
    {
        platform_->attach(platformNode_);
    }

    // Boxes
    {
        const int stepping = 10;
        const int layers = 10;
        const int amount = 20;
        const float R = 80.0f;

        for(int i = -layers/2; i < layers/2; ++i)
        {
            Graph::SceneNode* ringnode = cubeNode_->createSceneNodeChild();

            for(int j = 0; j < amount; ++j)
            {
                float angle = 2 * M_PI * j / static_cast<float>(amount);

                Graph::SceneNode* node = ringnode->createSceneNodeChild();
                node->setPosition(QVector3D(R * sin(angle), i * stepping, R * cos(angle)));
                node->attachEntity(cube_[abs(i + j) % 2].get());
                node->setShadowCaster(false);

                cubes_.push_back(node);
            }
        }
    }
}