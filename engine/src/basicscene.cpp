#include "basicscene.h"

#include "engine/scenenode.h"
#include "engine/entity.h"
#include "engine/cube.h"
#include "engine/mesh.h"

BasicScene::BasicScene()
    : camera_(QVector3D(0, 0, -5), 0.0f, 0.0f, 60.0f, 0.0f)
{
    torus_ = nullptr;
    oildrum_ = nullptr;
    sphere_ = nullptr;
    suzanne_ = nullptr;

    oildrumNode_ = nullptr;
    cubeNode_ = nullptr;
    monkeyNode_ = nullptr;
    torusNode_ = nullptr;
}

BasicScene::~BasicScene()
{
    if(torus_ != nullptr) delete torus_;
    if(oildrum_ != nullptr) delete oildrum_;
    if(sphere_ != nullptr) delete sphere_;
    if(suzanne_ != nullptr) delete suzanne_;

    for(Engine::Texture*  texture: textures_)
    {
        delete texture;
    }

    for(Engine::Renderable* renderable : renderables_)
    {
        delete renderable;
    }
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
    suzanne_ = new Engine::Mesh(funcs);
    if(!suzanne_->loadFromFile("./assets/suzanne.obj"))
    {
        qDebug() << "Failed to load suzanne!";
        delete suzanne_;
        suzanne_ = nullptr;
    }

    oildrum_ = new Engine::Mesh(funcs);
    if(!oildrum_->loadFromFile("./assets/oildrum.dae"))
    {
        qDebug() << "Failed to load oildrum!";
        delete oildrum_;
        oildrum_ = nullptr;
    }

    sphere_ = new Engine::Mesh(funcs);
    if(!sphere_->loadFromFile("./assets/sphere.obj"))
    {
        qDebug() << "Failed to load sphere!";
        delete sphere_;
        sphere_ = nullptr;
    }

    torus_ = new Engine::Mesh(funcs);
    if(!torus_->loadFromFile("./assets/torus.obj"))
    {
        qDebug() << "Failed to load torus!";
        delete torus_;
        torus_ = nullptr;
    }

    /*Engine::Cube* cube = new Engine::Cube(funcs);
    renderables_.push_back(cube);

    // Load textures
    for(int i = 1; i <= 2; ++i)
    {
        QString file = ":/images/wooden_crate" + QString::number(i) + ".png";

        Engine::Texture* tex = new Engine::Texture(funcs);
        if(!tex->loadFromFile(file))
        {
            qDebug() << "Failed to load texture: " << file;
        }

        else
        {
            tex->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
            tex->setWrap(GL_REPEAT, GL_REPEAT);
            tex->generateMipmaps();
            textures_.push_back(tex);
        }
    }

    // Add a bunch of cubes
    for(int y = -5; y < 5; ++y)
    {
        for(int z = 10; z < 20; ++z)
        {
            QMatrix4x4 mat;
            mat.translate(0, y * 5.0f, z * 5.0f);

            for(int x = -5; x < 5; ++x)
            {
                Engine::Texture* tex = textures_[(x + y + z) % textures_.size()];

                Engine::ModelInstance instance(&program_, cube, tex);
                instance.applyTransformation(mat);
                instance.setAmbientColor(QVector3D(0.0f, 0.0f, 0.02f));
                instance.setSpecularColor(QVector3D(0.2f, 0.2f, 0.2f));
                models_.push_back(instance);

                mat.translate(5.0f, 0, 0);
            }
        }
    }*/
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
        oildrumNode_->attachEntity(oildrum_);
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
        torusNode_->attachEntity(torus_);
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
        node->attachEntity(sphere_);
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
        monkeyNode_->attachEntity(suzanne_);
    }
}