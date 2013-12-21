// Scene updates and performs operations on the underlying hierarchial node structure
// that represents entities (lights and meshes) in our scene.

#ifndef SCENE_H
#define SCENE_H

#include "scenemodel.h"
#include "visiblescene.h"

#include "graph/scenenode.h"

#include <QSet>

namespace Engine {

class Scene : public SceneModel, public VisibleScene
{
public:
    Scene();
    virtual ~Scene();

    // Implemented methods from VisibleScene
    virtual void queryVisibles(const QMatrix4x4& viewProj, RenderQueue& renderQueue, bool shadowCasters = false);

    virtual Entity::Light* directionalLight() const;
    virtual CubemapTexture* skybox() const;

    virtual void addVisitor(BaseVisitor* visitor);
    virtual void removeVisitor(BaseVisitor* visitor);

    virtual Entity::Camera* camera() const;

    // Implemented methods from SceneModel
    virtual void setDirectionalLight(Entity::Light* light);
    virtual void setSkybox(const std::shared_ptr<CubemapTexture>& texture);

    virtual Graph::SceneNode* rootNode();

    virtual void setCamera(Entity::Camera* camera);
    virtual void update();

private:
    QSet<BaseVisitor*> visitors_;

    Graph::SceneNode rootNode_;
    std::shared_ptr<CubemapTexture> skybox_;

    Entity::Light* directionalLight_;
    Entity::Camera* camera_;

    void findVisibles(const QMatrix4x4& viewProj, Graph::SceneNode* node,
        RenderQueue& queue, bool shadowCasters);

    Scene(const Scene&);
    Scene& operator=(const Scene&);
};

}

#endif // SCENE_H