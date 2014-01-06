#include "freelookscene.h"

#include "inputstate.h"
#include "graph/scenenode.h"
#include "cubemapresource.h"
#include "scene/scenemanager.h"

#include <QCursor>

using namespace Engine;

FreeLookScene::FreeLookScene(Engine::ResourceDespatcher& despatcher)
    : despatcher_(despatcher), scene_(nullptr), speed_(15.0f), input_(nullptr),
    camera_(new Graph::Camera(Graph::Camera::PERSPECTIVE))
{
    camera_->setDirection(QVector3D(1, 0, 0));
    camera_->setPosition(QVector3D(0, 10, 0));
    camera_->setFarPlane(1000.0f);
    camera_->update();
}

FreeLookScene::~FreeLookScene()
{
    if(scene_ != nullptr)
    {
        scene_->eraseScene();
    }
}

void FreeLookScene::setFov(float angle)
{
    camera_->setFov(angle);
    camera_->update();
}

void FreeLookScene::setManager(Engine::SceneManager* model)
{
    scene_ = model;

    if(scene_ != nullptr)
    {
        scene_->addSceneLeaf(camera_);
        camera_->attach(&scene_->rootNode());

        initialise();
    }
}

void FreeLookScene::update(unsigned int elapsed)
{
    if(input_ == nullptr)
    {
        return;
    }

    const float mouseSpeed = 5.0f;

    const float elapsedMs = elapsed / 1000.0f;
    float distance = elapsedMs * speed_;

    QVector3D direction;

    if(input_->keyDown(Qt::Key::Key_W))
    {
        direction += camera_->direction();
    }

    if(input_->keyDown(Qt::Key::Key_S))
    {
        direction -= camera_->direction();
    }

    if(input_->keyDown(Qt::Key::Key_D))
    {
        direction += camera_->right();
    }

    if(input_->keyDown(Qt::Key::Key_A))
    {
        direction -= camera_->right();
    }

    if(direction.length() > 0)
    {
        camera_->move(direction.normalized() * distance);
    }

    QPoint delta = lastMouse_ - input_->mousePos();
    lastMouse_ = input_->mousePos();

    if(input_->keyDown(InputState::KEY_MOUSE_RIGHT))
    {
        camera_->yaw(mouseSpeed * elapsedMs * delta.x());
        camera_->pitch(mouseSpeed * elapsedMs * delta.y());
    }

    // Mouse wheel moves the camera up and down
    int wheel = input_->wheelDelta();
    if(wheel != 0)
    {
        camera_->move(QVector3D(0, speed_ * wheel / 1200.0f, 0));
    }

    camera_->update();
}

void FreeLookScene::setSkyboxTexture(const QString& fileName)
{
    CubemapResource::Ptr skybox = despatcher_.get<CubemapResource>(fileName, TC_SRGBA);
    skybox->setFiltering(GL_LINEAR, GL_LINEAR);

    scene_->setSkyboxCubemap(skybox);
}

Graph::SceneNode& FreeLookScene::rootNode()
{
    return scene_->rootNode();
}

ResourceDespatcher& FreeLookScene::despatcher()
{
    return despatcher_;
}

void FreeLookScene::setFlySpeed(float speed)
{
    speed_ = speed;
}

const QVector3D& FreeLookScene::playerPosition() const
{
    return camera_->position();
}

void FreeLookScene::setInput(InputState* input)
{
    input_ = input;
}

InputState* FreeLookScene::input()
{
    return input_;
}

Engine::Graph::Camera* FreeLookScene::camera()
{
    return camera_.get();
}

Engine::SceneManager& FreeLookScene::scene()
{
    return *scene_;
}

Graph::Light::Ptr FreeLookScene::createLight(Graph::Light::LightType type)
{
    Graph::Light::Ptr light(new Graph::Light(type));
    light->attach(&rootNode());

    scene_->addSceneLeaf(light);
    return light;
}