#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "renderer.h"

#include "gbuffer.h"
#include "technique/dsgeometryshader.h"
#include "technique/dsmaterialshader.h"
#include "renderqueue.h"
#include "renderable/quad.h"
#include "material.h"

#include <QRect>

namespace Engine {

class ResourceDespatcher;

class DeferredRenderer : public Renderer
{
public:
    DeferredRenderer(ResourceDespatcher* despatcher);
    virtual ~DeferredRenderer();

    // Sets OpenGL viewport parameters
    virtual bool setViewport(unsigned int width, unsigned int height, unsigned int samples,
        unsigned int left = 0, unsigned int top = 0);

    // Sets the scene to be used for future render calls
    // precondition: scene != nullptr
    virtual void setScene(VisibleScene* scene);

    // Sets the Postfx hook to be executed after the last rendering stage.
    // precondition: Viewport has been set
    // postcondition: hook is called after render() call, or cleared if postfx is null.
    //                If postfx->initialise returns false, the postfx hook is not installed.
    virtual bool setPostfxHook(Effect::Postfx* postfx);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Entity::Camera* camera);

    // TODO: Replace flags with a proper configuration interface
    virtual void setFlags(unsigned int flags);
    virtual unsigned int flags() const;

    GBuffer* getGBuffer();

private:
    QRect viewport_;
    unsigned int samples_;
    VisibleScene* scene_;
    Entity::Camera* camera_;

    GBuffer gbuffer_;
    Technique::DSGeometryShader geometryShader_;
    Technique::DSMaterialShader materialShader_;

    // Error material
    Material errorMaterial_;

    // Quad for screen-space rendering
    Renderable::Quad quad_;

    bool initialise(unsigned int width, unsigned int height, unsigned int samples);

    void geometryPass(const RenderQueue& queue);
    void preLightPass();
};

}

#endif // DEFERREDRENDERER_H