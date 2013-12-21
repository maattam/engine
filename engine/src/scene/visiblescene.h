// Interface class for performing scene visibility queries.

#ifndef VISIBLESCENE_H
#define VISIBLESCENE_H

#include <deque>
#include <QMatrix4x4>

class BaseVisitor;

namespace Engine {

namespace Entity {
    class Light;
    class Camera;
}

class CubemapTexture;
class RenderQueue;

class VisibleScene
{
public:
    virtual ~VisibleScene() {};

    // Queries list of potentially visible objects within the view frustrum and populates the renderQueue.
    // For each potentially visible entity, the visitees are accepted.
    // If shadowCasters is true, adds only shadow casting entities to the queue.
    virtual void queryVisibles(const QMatrix4x4& viewProj, RenderQueue& renderQueue, bool shadowCasters = false) = 0;

    // Adds an entity visitor, which will be called for culled entities.
    // If the visitor already exists, it won't be duplicated.
    // precondition: visitor != nullptr
    virtual void addVisitor(BaseVisitor* visitor) = 0;

    // Removes a visitor from the visitor list.
    virtual void removeVisitor(BaseVisitor* visitor) = 0;

    // Directional light can be null
    virtual Entity::Light* directionalLight() const = 0;

    // Skybox can be null
    virtual CubemapTexture* skybox() const = 0;

    virtual Entity::Camera* camera() const = 0;
};

}

#endif // VISIBLESCENE_H