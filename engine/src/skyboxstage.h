#ifndef SKYBOXSTAGE_H
#define SKYBOXSTAGE_H

#include "renderstage.h"

#include <memory>

namespace Engine {

namespace Technique {
    class Skybox;
}

namespace Renderable {
    class Renderable;
}

class GBuffer;

class SkyboxStage : public RenderStage
{
public:
    explicit SkyboxStage(Renderer* renderer);
    virtual ~SkyboxStage();

    // Sets the scene to be used for future render calls
    // precondition: scene != nullptr
    virtual void setScene(VisibleScene* scene);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render(Entity::Camera* camera);

    // Renders the scene to a render target instead of the default surface.
    // If fbo is nullptr, the default framebuffer (0) is used.
    virtual void setOutputFBO(QOpenGLFramebufferObject* fbo);

    typedef std::shared_ptr<Technique::Skybox> SkyboxPtr;
    typedef std::shared_ptr<Renderable::Renderable> MeshPtr;

    // Sets skybox technique for rendering the skybox.
    // precondition: skybox != nullptr
    // postcondition: Ownership is copied
    void setSkyboxTechnique(const SkyboxPtr& skybox);

    // Sets skybox mesh for rendering the skybox
    // postcondition: Ownership is copied
    void setSkyboxMesh(const MeshPtr& mesh);

    // If the renderer uses a gbuffer for storing geometry, the depth texture is used.
    // precondition: gbuffer != nullptr
    void setGBuffer(GBuffer const* gbuffer);

private:
    GBuffer const* gbuffer_;
    QOpenGLFramebufferObject* fbo_;
    VisibleScene* scene_;

    SkyboxPtr skybox_;
    MeshPtr mesh_;
};

}

#endif // SKYBOXSTAGE_H