// Interface class for performing scene visibility queries.

#ifndef VISIBLESCENE_H
#define VISIBLESCENE_H

#include <deque>
#include <QMatrix4x4>
#include <QVector3D>

namespace Engine {

namespace Entity {
    class Light;
}

class Material;
class CubemapTexture;

namespace Renderable {
    class Renderable;
}

typedef std::deque<std::pair<Material*, Renderable::Renderable*>> RenderList;

class VisibleScene
{
public:
    typedef std::pair<QMatrix4x4, RenderList> VisibleNode;
    typedef std::pair<QVector3D, Entity::Light*> VisibleLight;

    typedef std::deque<VisibleNode> RenderQueue;
    typedef std::deque<VisibleLight> Lights;

    virtual ~VisibleScene() {};

    // Queries list potentially visible objects within the view frustrum and populates the renderQueue.
    // If shadowCasters is true, adds only shadow casting entities to the queue.
    // queryVisibles also caches all the lights within the camera's frustrum if shadowCasters is false.
    virtual void queryVisibles(const QMatrix4x4& viewProj, RenderQueue& renderQueue, bool shadowCasters = false) = 0;

    // Retrivies all the lights within the cached frustrum that were resolved during the last queryVisibles.
    virtual const Lights& queryLights() const = 0;

    // Directional light can be null
    virtual Entity::Light* directionalLight() const = 0;

    // Skybox can be null
    virtual CubemapTexture* skybox() const = 0;
};

}

#endif // VISIBLESCENE_H