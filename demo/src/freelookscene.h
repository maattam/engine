// Represents a simple scene with a free look camera, directional light and skybox.

#ifndef FREELOOKSCENE_H
#define FREELOOKSCENE_H

#include "scene/scenecontroller.h"

#include "cubemaptexture.h"
#include "graph/light.h"
#include "graph/camera.h"
#include "resourcedespatcher.h"

#include <QPoint>

class InputState;

class FreeLookScene : public Engine::SceneController
{
public:
    explicit FreeLookScene(Engine::ResourceDespatcher* despatcher);
    virtual ~FreeLookScene();

    void setInput(InputState* input);

    // Camera parameters
    void setAspectRatio(float ratio);
    void setFov(float angle);

    const QVector3D& playerPosition() const;

    // Implemented methods from SceneController
    virtual void setModel(Engine::SceneModel* model);
    virtual void update(unsigned int elapsed);

protected:
    void setFlySpeed(float speed);
    void setSkyboxTexture(const QString& fileName);
    void setDirectionalLight(const QVector3D& color, const QVector3D& direction,
        float ambientIntensity, float diffuseIntensity);

    // Attaches the camera to the given node
    void attachCamera(Engine::Graph::SceneNode* node);

    Engine::Graph::SceneNode* rootNode();
    Engine::ResourceDespatcher* despatcher();

    InputState* input();
    Engine::Graph::Camera* camera();
    Engine::Graph::Light* directionalLight();

    // Called after scene model has been set and scenegraph can be populated.
    virtual void initialise() = 0;

private:
    Engine::ResourceDespatcher* despatcher_;
    Engine::SceneModel* scene_;
    InputState* input_;
    float speed_;
    QPoint lastMouse_;

    Engine::Graph::Light dirLight_;
    std::shared_ptr<Engine::CubemapTexture> skybox_;
    Engine::Graph::Camera camera_;
};

#endif // FREELOOKSCENE_H