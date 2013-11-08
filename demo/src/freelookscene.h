// Represents a simple scene with a free look camera, directional light and skybox.

#ifndef FREELOOKSCENE_H
#define FREELOOKSCENE_H

#include "scene/scenecontroller.h"

#include "cubemaptexture.h"
#include "entity/light.h"
#include "entity/camera.h"
#include "resourcedespatcher.h"

class Input;

class FreeLookScene : public Engine::SceneController
{
public:
    explicit FreeLookScene(Engine::ResourceDespatcher* despatcher);
    virtual ~FreeLookScene();

    void setInput(Input* input);

    // Camera parameters
    void setAspectRatio(float ratio);
    void setFov(float angle);

    const QVector3D& playerPosition() const;

    // Implemented methods from SceneController
    virtual void setModel(Engine::SceneModel* model);
    virtual void renderScene();
    virtual void update(unsigned int elapsed);

protected:
    void setSkyboxTexture(const QString& fileName);
    void setDirectionalLight(const QVector3D& color, const QVector3D& direction,
        float ambientIntensity, float diffuseIntensity);

    // Attaches the camera to the given node
    void attachCamera(Engine::Graph::SceneNode* node);

    Engine::Graph::SceneNode* rootNode();
    Engine::ResourceDespatcher* despatcher();
    Input* const input() const;

    // Called after scene model has been set and scenegraph can be populated.
    virtual void initialise() = 0;

private:
    Engine::ResourceDespatcher* despatcher_;
    Engine::SceneModel* scene_;
    Input* input_;

    Engine::Entity::Light dirLight_;
    Engine::CubemapTexture::Ptr skybox_;
    Engine::Entity::Camera camera_;
};

#endif // FREELOOKSCENE_H