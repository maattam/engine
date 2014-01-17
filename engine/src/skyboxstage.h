//
//  Author   : Matti Määttä
//  Summary  : SkyboxStage fills the empty parts of the scene with skybox texture.
//

#ifndef SKYBOXSTAGE_H
#define SKYBOXSTAGE_H

#include "renderstage.h"
#include "scene/sceneobserver.h"

#include <memory>

namespace Engine {

namespace Technique {
    class Skybox;
}

namespace Renderable {
    class Renderable;
}

class GBuffer;
class CubemapTexture;

class SkyboxStage : public RenderStage, public SceneObserver
{
public:
    explicit SkyboxStage(Renderer* renderer);
    virtual ~SkyboxStage();

    // Sets the observable for the current scene.
    // precondition: observable != nullptr.
    virtual void setObservable(SceneObservable* observable);

    // Sets the camera for the current geometry batch.
    // precondition: camera != nullptr
    virtual void setCamera(Graph::Camera* camera);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render();

    // Renders the scene to a render target instead of the default surface.
    // If fbo is 0, the default framebuffer is used.
    virtual void setRenderTarget(GLuint fbo);

    typedef std::shared_ptr<Technique::Skybox> SkyboxPtr;
    typedef std::shared_ptr<Renderable::Renderable> MeshPtr;

    // Sets skybox mesh for rendering the skybox
    // postcondition: Ownership is copied
    void setSkyboxMesh(const MeshPtr& mesh);

    void setSkyboxTechnique(const SkyboxPtr& skybox);

    // If the renderer uses a gbuffer for storing geometry, the depth texture is used.
    // precondition: gbuffer != nullptr
    void setGBuffer(GBuffer const* gbuffer);

    // SceneObserver implementation.
    virtual void skyboxTextureUpdated(CubemapTexture* skybox);

private:
    GBuffer const* gbuffer_;
    GLuint fbo_;
    int cubemapUnit_;

    CubemapTexture* cubemap_;
    SkyboxPtr skybox_;
    MeshPtr mesh_;
    Graph::Camera* camera_;

    void initTechnique();
};

}

#endif // SKYBOXSTAGE_H