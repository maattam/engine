// Render stage for deferred shading lighting accumulation.

#ifndef LIGHTINGSTAGE_H
#define LIGHTINGSTAGE_H

#include "renderstage.h"
#include "visitor.h"

namespace Engine {

namespace Entity {
    class Light;
}

class LightingStage : public RenderStage,
    public BaseVisitor, public Visitor<Entity::Light>
{
public:
    explicit LightingStage(Renderer* renderer);
    virtual ~LightingStage();

    // Sets the scene to be used for future render calls
    // precondition: scene != nullptr
    virtual void setScene(VisibleScene* scene);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Entity::Camera* camera);

private:
    VisibleScene* scene_;
};

}

#endif // LIGHTINGSTAGE_H