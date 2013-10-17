// Scene updates and performs operations on the hierarchial node structure
// that presents entities (lights and meshes) in our scene.

#ifndef SCENE_H
#define SCENE_H

#include <QMatrix4x4>

#include "entity/entity.h"
#include "entity/frustrum.h"
#include "entity/light.h"
#include "graph/scenenode.h"
#include "cubemaptexture.h"

namespace Engine {

class Scene
{
public:
    typedef std::pair<QMatrix4x4, RenderList> VisibleNode;
    typedef std::deque<VisibleNode> RenderQueue;

    typedef std::deque<Entity::VisibleLight> Lights;

    Scene();
    virtual ~Scene();

    // Culls visible objects within the view frustrum and populates the renderQueue.
    // If shadowCasters is true, adds only shadow casting entities to the queue.
    // cullVisibles also caches all the lights within the camera's frustrum if shadowCasters is false.
    virtual void cullVisibles(const Entity::Frustrum& frustrum, RenderQueue& renderQueue, bool shadowCasters = false);

    // Retrivies all the lights within the cached frustrum that were resolved during the last cullVisibles.
    virtual const Lights& queryLights() const;

    // Directional light can be null
    Entity::Light* directionalLight() const;
    void setDirectionalLight(Entity::Light* light);

    const CubemapTexture::Ptr& skybox() const;
    void setSkybox(const CubemapTexture::Ptr& texture);

    Graph::SceneNode* rootNode();

private:
    Graph::SceneNode rootNode_;
    CubemapTexture::Ptr skybox_;

    Entity::Light* directionalLight_;
    Lights lights_;

    void findVisibles(const Entity::Frustrum& frustrum, Graph::SceneNode* node, const QMatrix4x4& worldView,
        Scene::RenderQueue& queue, bool shadowCasters);

    Scene(const Scene&);
    Scene& operator=(const Scene&);
};

}

#endif // SCENE_H