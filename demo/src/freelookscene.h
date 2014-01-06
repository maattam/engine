// Represents a simple scene with a free look camera, directional light and skybox.

#ifndef FREELOOKSCENE_H
#define FREELOOKSCENE_H

#include "cubemaptexture.h"
#include "graph/light.h"
#include "graph/camera.h"
#include "resourcedespatcher.h"

#include <QPoint>

namespace Engine {
    class SceneManager;
}

class InputState;

class FreeLookScene
{
public:
    explicit FreeLookScene(Engine::ResourceDespatcher& despatcher);
    virtual ~FreeLookScene();

    void setInput(InputState* input);

    // Camera parameters
    void setFov(float angle);

    const QVector3D& playerPosition() const;

    virtual void setManager(Engine::SceneManager* manager);
    virtual void update(unsigned int elapsed);

    void setFlySpeed(float speed);
    void setSkyboxTexture(const QString& fileName);

    // Helper functions which create a new leaf, add it the scene
    // and attache it to the root node.
    Engine::Graph::Light::Ptr createLight(Engine::Graph::Light::LightType type);

protected:
    Engine::Graph::SceneNode& rootNode();
    Engine::ResourceDespatcher& despatcher();

    InputState* input();
    Engine::SceneManager& scene();

    Engine::Graph::Camera* camera();

    // Called after scene model has been set and scenegraph can be populated.
    virtual void initialise() = 0;

private:
    Engine::ResourceDespatcher& despatcher_;
    Engine::SceneManager* scene_;
    InputState* input_;
    float speed_;
    QPoint lastMouse_;

    Engine::Graph::Camera::Ptr camera_;
};

#endif // FREELOOKSCENE_H