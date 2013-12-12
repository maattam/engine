// Interface view class for implementing different renderers.

#ifndef RENDERER_H
#define RENDERER_H

namespace Engine {

class VisibleScene;

namespace Entity {
    class Camera;
}

namespace Effect {
    class Postfx;
}

class Renderer
{
public:
    virtual ~Renderer() {};

    // Sets OpenGL viewport parameters and initialises buffers
    // postcondition: true on success, viewport set and buffers initialised
    virtual bool setViewport(unsigned int width, unsigned int height, unsigned int samples,
        unsigned int left = 0, unsigned int top = 0) = 0;

    // Sets the scene to be used for future render calls
    // precondition: scene != nullptr
    virtual void setScene(VisibleScene* scene) = 0;

    // Sets the Postfx hook to be executed after the last rendering stage.
    // precondition: Viewport has been set
    // postcondition: hook is called after render() call, or cleared if postfx is null.
    //                If postfx->initialise returns false, the postfx hook is not installed.
    virtual bool setPostfxHook(Effect::Postfx* postfx) = 0;

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Entity::Camera* camera) = 0;

    // TODO: Replace flags with a proper configuration interface
    virtual void setFlags(unsigned int flags) = 0;
    virtual unsigned int flags() const = 0;
};

};

#endif // RENDERER_H