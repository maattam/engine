#include "basicscene.h"

#include "engine/scenenode.h"
#include "engine/entity.h"
#include "engine/cube.h"
#include "engine/mesh.h"

const float PI = 3.14159;

BasicScene::BasicScene()
    : camera_(QVector3D(-2, 6, 7), 0, 0.0f, 60.0f, 0.0f)
{
    time_ = 0;
}

BasicScene::~BasicScene()
{
}

const Engine::DirectionalLight& BasicScene::queryDirectionalLight()
{
    return directionalLight_;
}

const std::vector<Engine::PointLight>& BasicScene::queryPointLights()
{
    return pointLights_;
}

const std::vector<Engine::SpotLight>& BasicScene::querySpotLights()
{
    return spotLights_;
}

void BasicScene::initialize(QOpenGLFunctions_4_2_Core* funcs)
{
    // Load all our stuff
    gl = funcs;

    // Set up directional light
    directionalLight_.diffuseIntensity = 0.2f;
    directionalLight_.direction = QVector3D(1.0f, -1.0f, -1.0f);
    //directionalLight_.color = QVector3D(0, 0, 0);

    // Set up point lights
    Engine::PointLight pointLight;
    pointLight.diffuseIntensity = 15.0f;
    pointLight.attenuation.exp = 0.1f;

    pointLights_.push_back(pointLight);

    pointLight.diffuseIntensity = 150.0f;
    pointLight.color = QVector3D(0.0f, 0.0f, 1.0f);
    pointLight.attenuation.exp = 0.05f;
    pointLights_.push_back(pointLight);

    // Set up spot lights
    Engine::SpotLight spotLight;
    spotLight.color = QVector3D(1.0f, 0.0f, 1.0f);
    spotLight.position = 2*QVector3D(-6.0f, 2.0f, 8.0f);
    spotLight.direction = QVector3D(4.0f, 0.0f, -8.0f);
    spotLight.diffuseIntensity = 25.0f;
    spotLight.attenuation.exp = 0.05f;
    spotLight.cutoff = 20.0f;
    spotLights_.push_back(spotLight);

    // Load models
    platform_ = std::make_shared<Engine::Mesh>(funcs);
    if(!platform_->loadFromFile("./assets/blocks.dae"))
    {
        qDebug() << "Failed to load platform!";
    }

    oildrum_ = std::make_shared<Engine::Mesh>(funcs);
    if(!oildrum_->loadFromFile("./assets/oildrum.dae"))
    {
        qDebug() << "Failed to load oildrum!";
    }

    sphere_ = std::make_shared<Engine::Mesh>(funcs);
    if(!sphere_->loadFromFile("./assets/sphere.obj"))
    {
        qDebug() << "Failed to load sphere!";
    }

    torus_ = std::make_shared<Engine::Mesh>(funcs);
    if(!torus_->loadFromFile("./assets/torus.obj"))
    {
        qDebug() << "Failed to load torus!";
    }


    // Load cubes
    for(int i = 0; i < 2; ++i)
    {
        std::string file = "./assets/wooden_crate" + QString::number(i+1).toStdString() + ".png";

        Engine::Texture::Ptr tex = std::make_shared<Engine::Texture>(funcs);
        if(!tex->loadFromFile(file))
        {
            qDebug() << "Failed to load texture: " << file.c_str();
        }

        else
        {
            tex->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
            tex->setWrap(GL_REPEAT, GL_REPEAT);
            tex->generateMipmaps();

            Engine::Material::Ptr mat = std::make_shared<Engine::Material>(funcs);
            mat->setSpecularIntensity(2.0f);
            mat->setTexture(Engine::Material::TEXTURE_DIFFUSE, tex);
            materials_.push_back(mat);

            cube_[i] = std::make_shared<Engine::BoxPrimitive>(funcs);
            cube_[i]->setMaterial(mat);
        }
    }
}

void BasicScene::update(float elapsed)
{
    const float R = 16.0f;
    const float R2 = 75.0f;
    time_ += elapsed * 1000;

    float angle = static_cast<float>(time_) / 1000;

    QVector3D lightPos = QVector3D(R/2 * sinf(angle * 3), R * sinf(angle), R * cosf(angle));

    pointLights_[1].position = QVector3D(R2 * sinf(angle), R2/2 * sinf(angle * 3), R2 * cosf(angle));

    pointLights_[0].position = lightPos;
    sphereNode_->setPosition(lightPos);

    QMatrix4x4 mat;
    mat.scale(0.2f);
    sphereNode_->applyTransformation(mat);

    mat.setToIdentity();
    mat.rotate(15.0f * elapsed, 1, 1, 0);
    torusNode_->applyTransformation(mat);

    mat.setToIdentity();
    mat.rotate(3.0f * elapsed, 0, 0, 1);
    platformNode_->applyTransformation(mat);

    for(int i = 0; i < cubeNode_->numChildren(); ++i)
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

    for(int i = 0; i < cubes_.size(); ++i)
    {
        mat.setToIdentity();
        
        if(i % 2 == 0)
            mat.rotate(15.0f * elapsed, 1, 1, 0);

        else
            mat.rotate(15.0f * elapsed, 1, 0, 0);

        cubes_[i]->applyTransformation(mat);
    }
}

Engine::Camera* BasicScene::activeCamera()
{
    // We only have a single camera
    return &camera_;
}

void BasicScene::prepareScene(Engine::SceneNode* scene)
{
    AbstractScene::prepareScene(scene);

    // Create nodes
    platformNode_ = dynamic_cast<Engine::SceneNode*>(scene->createChild());
    oildrumNode_ = dynamic_cast<Engine::SceneNode*>(platformNode_->createChild());
    torusNode_ = dynamic_cast<Engine::SceneNode*>(scene->createChild());
    cubeNode_ = dynamic_cast<Engine::SceneNode*>(scene->createChild());
    sphereNode_ = dynamic_cast<Engine::SceneNode*>(scene->createChild());

    // Oildrum
    {
        Engine::Material::Attributes attrib;
        attrib.shininess = 12.0f;
        attrib.specularIntensity = 1.0f;

        oildrum_->setMaterialAttributes(attrib);

        QMatrix4x4 mat;
        mat.translate(-4, -2, 1);

        Engine::SceneNode* node = dynamic_cast<Engine::SceneNode*>(oildrumNode_->createChild());

        node->applyTransformation(mat);
        node->attachEntity(oildrum_.get());

        node = dynamic_cast<Engine::SceneNode*>(oildrumNode_->createChild());
        mat.translate(6, -1, 0);
        node->applyTransformation(mat);
        node->attachEntity(oildrum_.get());
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

        // Platform light
        mat.setToIdentity();
        mat.translate(2*QVector3D(-6.0f, 2.0f, 8.0f));
        mat.scale(0.1f);

        auto node = dynamic_cast<Engine::SceneNode*>(scene->createChild());
        node->applyTransformation(mat);
        node->attachEntity(sphere_.get());
    }

    // platform
    {
        Engine::Material::Attributes attrib;
        //attrib.diffuseColor = QVector3D(0.2f, 0.0f, 0.8f);
        attrib.specularIntensity = 1.0f;
        attrib.shininess = 50.0f;

        platform_->setMaterialAttributes(attrib);

        QMatrix4x4 mat;
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
            Engine::SceneNode* ringnode = dynamic_cast<Engine::SceneNode*>(cubeNode_->createChild());

            for(int j = 0; j < amount; ++j)
            {
                float angle = 2 * PI * j / amount;

                QMatrix4x4 mat;
                mat.translate(R * sin(angle), i * stepping, R * cos(angle));

                Engine::SceneNode* node = dynamic_cast<Engine::SceneNode*>(ringnode->createChild());
                node->applyTransformation(mat);
                node->attachEntity(cube_[abs((i + j)) % 2].get());

                cubes_.push_back(node);
            }
        }
    }
}