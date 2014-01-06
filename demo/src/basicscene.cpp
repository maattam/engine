#include "basicscene.h"

#include "graph/scenenode.h"
#include "resourcedespatcher.h"
#include "texture2dresource.h"
#include "renderable/primitive.h"
#include "renderable/cube.h"
#include "scene/scenemanager.h"

#include <qmath.h>
#include <QDebug>

using namespace Engine;

BasicScene::BasicScene(ResourceDespatcher& despatcher)
    : FreeLookScene(despatcher), QObject(), time_(0)
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
}

void BasicScene::resourceInitialized(const QString& name)
{
    if(name == "assets/oildrum.dae")
    {
        Graph::Geometry::Ptr result = oildrum_->findLeaf<Graph::Geometry>("Oildrum-ref");
        if(result != nullptr)
        {
            Graph::SceneNode* node = result->parentNode()->createChild();
            node->setPosition(QVector3D(6, -2, 0));

            std::shared_ptr<Graph::SceneLeaf> cloneDrum = result->clone();
            scene().addSceneLeaf(cloneDrum);
            cloneDrum->attach(node);

        }
    }

    else if(name == "assets/sphere.obj")
    {
        Graph::Geometry::Ptr result = sphere_->findLeaf<Graph::Geometry>("Sphere");
        if(result != nullptr)
        {
            // Platform spot light
            Graph::SceneNode* node = rootNode().createChild();
            node->setPosition(2*QVector3D(-6.0f, 7/2, 6.0f));
            node->setScale(0.1f);
            node->setLightMask(0);

            std::shared_ptr<Graph::SceneLeaf> cloneSphere = result->clone();
            scene().addSceneLeaf(cloneSphere);

            cloneSphere->attach(node);
            lights_[2]->attach(node);
        }
    }
}

void BasicScene::initialise()
{
    // Load skybox
    setSkyboxTexture("assets/skybox/space/space*.png");

    // Set up directional light
    Graph::Light::Ptr dirLight = createLight(Graph::Light::LIGHT_DIRECTIONAL);
    dirLight->setColor(QVector3D(1, 1, 1));
    dirLight->setDirection(QVector3D(1.0f, -1.0f, -1.0f));
    dirLight->setAmbientIntensity(0.0f);
    dirLight->setDiffuseIntensity(0.1f);

    // Set up point lights
    Graph::Light::Ptr pointLight = createLight(Graph::Light::LIGHT_POINT);
    pointLight->setDiffuseIntensity(10.0f);
    pointLight->setAttenuationQuadratic(0.1f);
    lights_.push_back(pointLight);

    pointLight = createLight(Graph::Light::LIGHT_POINT);
    pointLight->setColor(QVector3D(0.0f, 0.0f, 1.0f));
    pointLight->setAttenuationQuadratic(0.025f);
    pointLight->setDiffuseIntensity(100.0f);
    lights_.push_back(pointLight);

    blueLightNode_ = rootNode().createChild();
    pointLight->attach(blueLightNode_);

    // Set up spot lights
    Graph::Light::Ptr spotLight = createLight(Graph::Light::LIGHT_SPOT);
    spotLight->setColor(QVector3D(1.0f, 0.0f, 1.0f));
    spotLight->setDirection(QVector3D(4.0f, -4.0f, -6.0f));
    spotLight->setDiffuseIntensity(20.0f);
    spotLight->setAttenuationQuadratic(0.05f);
    spotLight->setAngleOuterCone(20.0f);
    lights_.push_back(spotLight);

    // Load models
    oildrum_ = despatcher().get<ImportedNode>("assets/oildrum.dae", scene());
    hellknight_ = despatcher().get<ImportedNode>("assets/hellknight/hellknight.md5mesh", scene());
    platform_ = despatcher().get<ImportedNode>("assets/blocks.dae", scene());
    sphere_ = despatcher().get<ImportedNode>("assets/sphere.obj", scene());
    torus_ = despatcher().get<ImportedNode>("assets/torus.obj", scene());

    connect(oildrum_.get(), &ImportedNode::initialized, this, &BasicScene::resourceInitialized);
    connect(sphere_.get(), &ImportedNode::initialized, this, &BasicScene::resourceInitialized);

    // Load cubes
    for(int i = 0; i < 2; ++i)
    {
        QString file = "assets/wooden_crate" + QString::number(i+1) + ".png";

        Material::TexturePtr tex = despatcher().get<Texture2DResource>(file, TC_SRGBA);
        if(tex != nullptr)
        {
            Engine::Material::Ptr mat(new Engine::Material);
            mat->setTexture(Engine::Material::TEXTURE_DIFFUSE, tex);

            cube_[i] = std::make_shared<Graph::Geometry>(Renderable::Primitive<Renderable::Cube>::instance(), mat);
        }
    }

    // Create nodes
    platformNode_ = rootNode().createChild();
    torusNode_ = rootNode().createChild();
    cubeNode_ = rootNode().createChild();
    sphereNode_ = rootNode().createChild();

    // Oildrum
    {
        // Create rotation relation
        Graph::SceneNode* platf = platformNode_->createChild();

        Graph::SceneNode* node = platf->createChild();
        node->rotate(90.0f, UNIT_X);
        node->setPosition(QVector3D(-5, 0, 2));
        oildrum_->attach(node);

        hkNode_ = platf->createChild();

        hkNode_->setPosition(QVector3D(-1, 0, -2));
        hkNode_->setScale(0.025f);
        hellknight_->attach(hkNode_);
    }

    // Torus
    {
        torusNode_->setScale(180.0f);
        torus_->attach(torusNode_);
        torusNode_->setLightMask(0);
    }

    // Sphere
    {
        sphereNode_->setScale(0.2f);
        sphere_->attach(sphereNode_);
        sphereNode_->setLightMask(0);
        lights_[0]->attach(sphereNode_);
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
            Graph::SceneNode* ringnode = cubeNode_->createChild();

            for(int j = 0; j < amount; ++j)
            {
                float angle = 2 * M_PI * j / static_cast<float>(amount);

                Graph::SceneNode* node = ringnode->createChild();
                node->setPosition(QVector3D(R * sin(angle), i * stepping, R * cos(angle)));
                node->setLightMask(0);

                std::shared_ptr<Graph::SceneLeaf> cube = cube_[abs(i + j) % 2]->clone();
                cube->attach(node);
                scene().addSceneLeaf(cube);

                cubes_.push_back(node);
            }
        }
    }
}