// Interface view class for implementing different renderers.

#ifndef RENDERER_H
#define RENDERER_H

namespace Engine {

class VisibleScene;

namespace Entity {
    class Camera;
}

class Renderer
{
public:
    virtual ~Renderer() {};

    // Sets OpenGL viewport parameters.
    virtual bool setViewport(unsigned int width, unsigned int height, unsigned int samples,
        unsigned int left = 0, unsigned int top = 0) = 0;

    // Sets the scene to be used for future render calls
    // precondition: scene != nullptr
    virtual void setScene(VisibleScene* scene) = 0;

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Entity::Camera* camera) = 0;

    virtual void setFlags(unsigned int flags) = 0;
    virtual unsigned int flags() const = 0;
};

};

#endif // RENDERER_H