#ifndef SKYBOXTECHNIQUE_H
#define SKYBOXTECHNIQUE_H

#include "technique.h"
#include "renderable/cube.h"

#include <QMatrix4x4>

namespace Engine {
    
class ResourceDespatcher;
class CubemapTexture;

namespace Entity {
    class Camera;
}
    
namespace Technique {

class Skybox : public Technique
{
public:
    Skybox(ResourceDespatcher* despatcher);

    // Renders skybox around given camera's view
    // Does nothing if either camera or texture is null.
    void render(Entity::Camera* camera, CubemapTexture* texture);

protected:
    virtual void init();

private:
    GLuint mvpLocation_;
    GLuint samplerLocation_;

    Renderable::Cube mesh_;
};

}}

#endif // SKYBOXTECHNIQUE_H