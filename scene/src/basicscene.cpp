#include "basicscene.h"

#include "graph/scenenode.h"
#include "entity/entity.h"
#include "renderable/cube.h"
#include "entity/mesh.h"

#include <qmath.h>

using namespace Engine;

BasicScene::BasicScene()
    : camera_(QVector3D(-2, 6, 7), 0, 0.0f, 60.0f, 0.0f)
{
    time_ = 0;
}

BasicScene::~BasicScene()
{
}

const Entity::DirectionalLight& BasicScene::queryDirectionalLight()
{
    return directionalLight_;
}

const std::vector<Entity::PointLight>& BasicScene::queryPointLights()
{
    return pointLights_;
}

const std::vector<Entity::SpotLight>& BasicScene::querySpotLights()
{
    return spotLights_;
}

Entity::Camera* BasicScene::activeCamera()
{
    // We only have a single camera
    return &camera_;
}

void BasicScene::initialize(QOPENGL_FUNCTIONS* funcs)
{
    // Load all our stuff
    gl = funcs;

    // Set up directional light
    directionalLight_.diffuseIntensity = 0.2f;
    directionalLight_.direction = QVector3D(1.0f, -1.0f, -1.0f);
    //directionalLight_.color = QVector3D(0, 0, 0);

    // Set up point lights
    Entity::PointLight pointLight;
    pointLight.diffuseIntensity = 10.0f;
    pointLight.attenuation.exp = 0.1f;

    pointLights_.push_back(pointLight);

    pointLight.diffuseIntensity = 50.0f;
    pointLight.color = QVector3D(0.0f, 0.0f, 1.0f);
    pointLight.attenuation.exp = 0.025f;
    pointLights_.push_back(pointLight);

    // Set up spot lights
    Entity::SpotLight spotLight;
    spotLight.color = QVector3D(1.0f, 0.0f, 1.0f);
    spotLight.position = 2*QVector3D(-6.0f, 4.0f, 6.0f);
    spotLight.direction = QVector3D(4.0f, -4.0f, -6.0f);
    spotLight.diffuseIntensity = 25.0f;
    spotLight.attenuation.exp = 0.05f;
    spotLight.cutoff = 20.0f;
    spotLights_.push_back(spotLight);

    // Load models
    oildrum_ = std::make_shared<Entity::Mesh>(funcs);
    if(!oildrum_->loadFromFile("./assets/oildrum.dae"))
    {
        qDebug() << "Failed to load oildrum!";
    }

    hellknight_ = std::make_shared<Entity::Mesh>(funcs);
    if(!hellknight_->loadFromFile("./assets/hellknight/hellknight.md5mesh"))
    {
        qDebug() << "Failed to load hellknight!";
    }

    platform_ = std::make_shared<Entity::Mesh>(funcs);
    if(!platform_->loadFromFile("./assets/blocks.dae"))
    {
        qDebug() << "Failed to load platform!";
    }

    sphere_ = std::make_shared<Entity::Mesh>(funcs);
    if(!sphere_->loadFromFile("./assets/sphere.obj"))
    {
        qDebug() << "Failed to load sphere!";
    }

    torus_ = std::make_shared<Entity::Mesh>(funcs);
    if(!torus_->loadFromFile("./assets/torus.obj"))
    {
        qDebug() << "Failed to load torus!";
    }

    // Load cubes
    for(int i = 0; i < 2; ++i)
    {
        std::string file = "./assets/wooden_crate" + QString::number(i+1).toStdString() + ".png";

        Texture::Ptr tex = std::make_shared<Texture>(funcs);
        if(!tex->loadFromFile(file))
        {
            qDebug() << "Failed to load texture: " << file.c_str();
        }

        else
        {
            Engine::Material::Ptr mat = std::make_shared<Engine::Material>(funcs);
            mat->setSpecularIntensity(2.0f);
            mat->setTexture(Engine::Material::TEXTURE_DIFFUSE, tex);
            materials_.push_back(mat);

            cube_[i] = std::make_shared<Entity::BoxPrimitive>(funcs);
            cube_[i]->setMaterial(mat);
        }
    }
}

void BasicScene::update(unsigned int elapsedMs)
{
    const float R = 16.0f;
    const float R2 = 75.0f;

    float elapsed = static_cast<float>(elapsedMs) / 1000;
    float angle = static_cast<float>(time_) / 1000;

    time_ += elapsedMs;

    QVector3D lightPos = QVector3D(R/2 * sinf(angle * 3), R * sinf(angle), R * cosf(angle));

    pointLights_[1].position = QVector3D(R2 * sinf(angle), R2/2 * sinf(angle * 3), R2 * cosf(angle));

    pointLights_[0].position = lightPos;
    sphereNode_->setPosition(lightPos);

    QMatrix4x4 mat;
    // setPosition resets scale matrix so restore it
    mat.scale(0.2f);
    sphereNode_->applyTransformation(mat);

    mat.setToIdentity();
    mat.rotate(15.0f * elapsed, 1, 1, 0);
    torusNode_->applyTransformation(mat);

    mat.setToIdentity();
    mat.rotate(3.0f * elapsed, 0, 0, 1);
    platformNode_->applyTransformation(mat);

    for(size_t i = 0; i < cubeNode_->numChildren(); ++i)
    {
        if(i % 2 == 0)
        {
            mat.setToIdentity();
            mat.rotate(-5.0f * elapsed, 0, 1, 0);
            cubeNode_->getChild(i)->applyTransformation(mat);
        }
    }

    mat.setToIdentity();
    mat.rotate(-3.0f * elapsed, 0, 1, 0);
    cubeNode_->applyTransformation(mat);

    for(size_t i = 0; i < cubes_.size(); ++i)
    {
        mat.setToIdentity();
        
        if(i % 2 == 0)
            mat.rotate(15.0f * elapsed, 1, 1, 0);

        else
            mat.rotate(15.0f * elapsed, 1, 0, 0);

        cubes_[i]->applyTransformation(mat);
    }
}

void BasicScene::prepareScene(Graph::SceneNode* scene)
{
    AbstractScene::prepareScene(scene);

    // Create nodes
    platformNode_ = dynamic_cast<Graph::SceneNode*>(scene->createChild());
    torusNode_ = dynamic_cast<Graph::SceneNode*>(scene->createChild());
    cubeNode_ = dynamic_cast<Graph::SceneNode*>(scene->createChild());
    sphereNode_ = dynamic_cast<Graph::SceneNode*>(scene->createChild());

    // Oildrum
    {
        QMatrix4x4 mat;
        mat.translate(-4, -2, 1);

        Graph::SceneNode* node = dynamic_cast<Graph::SceneNode*>(platformNode_->createChild());

        node->applyTransformation(mat);
        node->attachEntity(oildrum_.get());

        node = dynamic_cast<Graph::SceneNode*>(platformNode_->createChild());

        mat.translate(6, -1, 0);
        node->applyTransformation(mat);
        node->attachEntity(oildrum_.get());

        node = dynamic_cast<Graph::SceneNode*>(platformNode_->createChild());

        mat.setToIdentity();
        mat.translate(-1, 2, 1);
        mat.scale(0.025f);

        Engine::Material::Attributes attrib;
        attrib.specularIntensity = 2.0f;

        hellknight_->setMaterialAttributes(attrib);

        node->applyTransformation(mat);
        node->attachEntity(hellknight_.get());
    }

    // Torus
    {
        Engine::Material::Attributes attrib;
        attrib.diffuseColor = QVector3D(0.2f, 0.3f, 0.8f);
        attrib.ambientColor = QVector3D(0.0f, 0.0f, 0.05f);
        attrib.specularIntensity = 10.0f;

        torus_->setMaterialAttributes(attrib);

        QMatrix4x4 mat;
        mat.scale(180.0f);

        torusNode_->applyTransformation(mat);
        torusNode_->attachEntity(torus_.get());
        torusNode_->setShadowCaster(false);
    }

    // Sphere
    {
        Engine::Material::Attributes attrib;
        attrib.diffuseColor = QVector3D(0.0f, 0.0f, 0.0f);
        attrib.ambientColor = QVector3D(2.0f, 2.0f, 2.0f);
        attrib.specularIntensity = 0.0f;

        sphere_->setMaterialAttributes(attrib);

        QMatrix4x4 mat;
        mat.scale(0.2f);

        sphereNode_->applyTransformation(mat);
        sphereNode_->attachEntity(sphere_.get());
        sphereNode_->setShadowCaster(false);

        // Platform light
        mat.setToIdentity();
        mat.translate(2*QVector3D(-6.0f, 4.0f, 6.0f));
        mat.scale(0.1f);

        Graph::SceneNode* node = dynamic_cast<Graph::SceneNode*>(scene->createChild());
        node->applyTransformation(mat);
        node->attachEntity(sphere_.get());
        node->setShadowCaster(false);
    }

    // platform
    {
        Engine::Material::Attributes attrib;
        //attrib.diffuseColor = QVector3D(0.2f, 0.0f, 0.8f);
        attrib.specularIntensity = 1.0f;
        attrib.shininess = 50.0f;

        platform_->setMaterialAttributes(attrib);

        QMatrix4x4 mat;
        // dae expects z to be upwards so we rotate the model by pi/4 along its x-axis
        mat.rotate(-90.0f, 1, 0, 0);

        platformNode_->applyTransformation(mat);
        platformNode_->attachEntity(platform_.get());
    }

    // Boxes
    {
        const int stepping = 10;
        const int layers = 10;
        const int amount = 20;
        const float R = 80.0f;

        for(int i = -layers/2; i < layers/2; ++i)
        {
            Graph::SceneNode* ringnode = dynamic_cast<Graph::SceneNode*>(cubeNode_->createChild());

            for(int j = 0; j < amount; ++j)
            {
                float angle = 2 * M_PI * j / static_cast<float>(amount);

                QMatrix4x4 mat;
                mat.translate(R * sin(angle), i * stepping, R * cos(angle));

                Graph::SceneNode* node = dynamic_cast<Graph::SceneNode*>(ringnode->createChild());
                node->applyTransformation(mat);
                node->attachEntity(cube_[abs(i + j) % 2].get());
                node->setShadowCaster(false);

                cubes_.push_back(node);
            }
        }
    }
}