#include "freelookscene.h"

#include "input.h"

using namespace Engine;

FreeLookScene::FreeLookScene(Engine::ResourceDespatcher* despatcher)
    : despatcher_(despatcher), scene_(nullptr),
    dirLight_(Entity::Light::LIGHT_DIRECTIONAL), camera_(Entity::Camera::PERSPECTIVE)
{
    camera_.setDirection(QVector3D(1, 0, 0));
    camera_.setPosition(QVector3D(0, 10, 0));
    camera_.update();
}

FreeLookScene::~FreeLookScene()
{
}

void FreeLookScene::setAspectRatio(float ratio)
{
    camera_.setAspectRatio(ratio);
    camera_.update();
}

void FreeLookScene::setFov(float angle)
{
    camera_.setFov(angle);
    camera_.update();
}

void FreeLookScene::setModel(Engine::SceneModel* model)
{
    scene_ = model;

    if(scene_ != nullptr)
    {
        scene_->setDirectionalLight(&dirLight_);
        initialise();
    }
}

void FreeLookScene::renderScene()
{
    if(scene_ != nullptr)
    {
        scene_->renderScene(&camera_);
    }
}

void FreeLookScene::update(unsigned int elapsed)
{
    if(input_ == nullptr)
    {
        return;
    }

    const float speed = 15.0f;
    const float mouseSpeed = 5.0f;

    const float elapsedMs = elapsed / 1000.0f;
    float distance = elapsedMs * speed;

    if(input_->keyDown(Qt::Key::Key_W))
    {
        camera_.move(camera_.direction() * distance);
    }

    if(input_->keyDown(Qt::Key::Key_S))
    {
        camera_.move(-camera_.direction() * distance);
    }

    if(input_->keyDown(Qt::Key::Key_D))
    {
        camera_.move(camera_.right() * distance);
    }

    if(input_->keyDown(Qt::Key::Key_A))
    {
        camera_.move(-camera_.right() * distance);
    }

    if(input_->keyDown(Input::KEY_MOUSE_RIGHT))
    {
        QPoint delta = input_->mouseDelta();

        camera_.yaw(mouseSpeed * elapsedMs * delta.x());
        camera_.pitch(mouseSpeed * elapsedMs * delta.y());
    }

    // Mouse wheel moves the camera up and down
    if(input_->wheelDelta() != 0)
    {
        camera_.move(QVector3D(0, input_->wheelDelta() / 100.0f, 0));
    }

    camera_.update();
}

void FreeLookScene::setSkyboxTexture(const QString& fileName)
{
    skybox_ = despatcher_->get<CubemapTexture>(fileName);
    skybox_->setFiltering(GL_LINEAR, GL_LINEAR);

    scene_->setSkybox(skybox_);
}

void FreeLookScene::setDirectionalLight(const QVector3D& color, const QVector3D& direction,
                                        float ambientIntensity, float diffuseIntensity)
{
    dirLight_.setColor(color);
    dirLight_.setDirection(direction);
    dirLight_.setAmbientIntensity(ambientIntensity);
    dirLight_.setDiffuseIntensity(diffuseIntensity);
}

Graph::SceneNode* FreeLookScene::rootNode()
{
    if(scene_ == nullptr)
    {
        return nullptr;
    }

    return scene_->rootNode();
}

ResourceDespatcher* FreeLookScene::despatcher()
{
    return despatcher_;
}

const QVector3D& FreeLookScene::playerPosition() const
{
    return camera_.position();
}

void FreeLookScene::setInput(Input* input)
{
    input_ = input;
}

Input* const FreeLookScene::input() const
{
    return input_;
}