#include "basicscene.h"

#include "engine/scenenode.h"
#include "engine/entity.h"
#include "engine/cube.h"
#include "engine/mesh.h"

#include <QTime>

BasicScene::BasicScene()
    : camera_(QVector3D(0, 0, 0), 0.0f, 0.0f, 60.0f, 0.0f)
{
}

BasicScene::~BasicScene()
{
}

Engine::Light* BasicScene::activeLight()
{
    return &lights_[0];
}

void BasicScene::initialize(QOpenGLFunctions_4_2_Core* funcs)
{
    // Load all our stuff
    gl = funcs;

    // Add light
    lights_.push_back(Engine::Light(QVector3D(1, 1, 1), QVector3D(0, 3, 0), 15.0f));

    // Load models
    suzanne_ = std::make_shared<Engine::Mesh>(funcs);
    if(!suzanne_->loadFromFile("./assets/suzanne.obj"))
    {
        qDebug() << "Failed to load suzanne!";
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
        std::string file = ":/images/wooden_crate" + QString::number(i+1).toStdString() + ".png";

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
            mat->setAmbientColor(QVector3D(0, 0, 0.05f));
            mat->setSpecularColor(QVector3D(0.2f, 0.2f, 0.2f));
            mat->setTexture(Engine::Material::TEXTURE_DIFFUSE, tex);
            materials_.push_back(mat);

            cube_[i] = std::make_shared<Engine::BoxPrimitive>(funcs);
            cube_[i]->setMaterial(mat);
        }
    }
}

void BasicScene::update(float elapsed)
{
    QMatrix4x4 mat;
    mat.rotate(15.0f * elapsed, 0, 0, 1);

    oildrumNode_->applyTransformation(mat);

    mat.setToIdentity();
    mat.rotate(15.0f * elapsed, 1, 1, 0);

    torusNode_->applyTransformation(mat);

    mat.setToIdentity();
    mat.rotate(15.0f * elapsed, 0, 1, 0);

    monkeyNode_->applyTransformation(mat);

    for(int i = 0; i < cubeNode_->numChildren(); ++i)
    {
        if(i % 2 == 0)
        {
            mat.setToIdentity();
            mat.rotate(-5.0f * elapsed, 0, 1, 0);
            cubeNode_->getChild(i)->applyTransformation(mat);
        }
    }

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
    oildrumNode_ = dynamic_cast<Engine::SceneNode*>(scene->createChild());
    monkeyNode_ = dynamic_cast<Engine::SceneNode*>(scene->createChild());
    torusNode_ = dynamic_cast<Engine::SceneNode*>(scene->createChild());
    cubeNode_ = dynamic_cast<Engine::SceneNode*>(scene->createChild());

    // Oildrum
    {
        Engine::Material::Attributes attrib;
        attrib.ambientColor = QVector3D(0.0f, 0.0f, 0.02f);
        attrib.specularColor = QVector3D(0.2f, 0.2f, 0.2f);

        oildrum_->setMaterialAttributes(attrib);

        QMatrix4x4 mat;
        mat.rotate(-90.0f, 1, 0, 0);
        mat.translate(15, 5, 0);

        oildrumNode_->applyTransformation(mat);
        oildrumNode_->attachEntity(oildrum_.get());
    }

    // Torus
    {
        Engine::Material::Attributes attrib;
        attrib.diffuseColor = QVector3D(0.2f, 0.3f, 0.7f);
        attrib.ambientColor = QVector3D(0.0f, 0.0f, 0.05f);
        attrib.specularColor = QVector3D(0.1, 0.1, 0.1);

        torus_->setMaterialAttributes(attrib);

        QMatrix4x4 mat;
        mat.scale(150.0f);

        torusNode_->applyTransformation(mat);
        torusNode_->attachEntity(torus_.get());
    }

    // Sphere
    {
        Engine::Material::Attributes attrib;
        attrib.diffuseColor = QVector3D(0.0f, 0.0f, 0.0f);
        attrib.ambientColor = QVector3D(2.0f, 2.0f, 2.0f);
        attrib.specularColor = QVector3D(0, 0, 0);

        sphere_->setMaterialAttributes(attrib);

        QMatrix4x4 mat;
        mat.translate(0, 3, 0);
        mat.scale(0.2f);

        Engine::SceneNode* node = dynamic_cast<Engine::SceneNode*>(scene->createChild());
        node->applyTransformation(mat);
        node->attachEntity(sphere_.get());
    }

    // Suzanne
    {
        Engine::Material::Attributes attrib;
        attrib.diffuseColor = QVector3D(0.2f, 0.0f, 0.8f);
        attrib.ambientColor = QVector3D(0.0f, 0.0f, 0.02f);
        attrib.specularColor = QVector3D(0.5f, 0.3f, 0.3f);

        suzanne_->setMaterialAttributes(attrib);

        QMatrix4x4 mat;
        mat.translate(5, 0, -10);
        mat.scale(2.0f);

        monkeyNode_->applyTransformation(mat);
        monkeyNode_->attachEntity(suzanne_.get());
    }

    // Boxes
    {
        const int stepping = 10;
        const int layers = 10;
        const int amount = 20;
        const float R = 80.0f;
        const float PI = 3.14159;

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