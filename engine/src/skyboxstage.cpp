#include "skyboxstage.h"

#include "renderable/renderable.h"
#include "technique/skybox.h"
#include "scene/visiblescene.h"
#include "cubemaptexture.h"
#include "graph/camera.h"
#include "gbuffer.h"

using namespace Engine;

SkyboxStage::SkyboxStage(Renderer* renderer)
    : RenderStage(renderer), gbuffer_(nullptr), fbo_(0), scene_(nullptr), cubemapUnit_(0)
{
}

SkyboxStage::~SkyboxStage()
{
}

void SkyboxStage::setScene(VisibleScene* scene)
{
    RenderStage::setScene(scene);
    scene_ = scene;
}

void SkyboxStage::setGBuffer(GBuffer const* gbuffer)
{
    gbuffer_ = gbuffer;
    initTechnique();
}

void SkyboxStage::render(Graph::Camera* camera)
{
    RenderStage::render(camera);

    if(scene_->skybox() == nullptr)
    {
        return; // Skybox now set
    }

    if(mesh_ == nullptr || skybox_ == nullptr || !skybox_->enable())
    {
        return;
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    // Translate the skybox mesh to view origin
    QMatrix4x4 trans;
    trans.translate(camera->position());
    skybox_->setMVP(camera->worldView() * trans);

    if(scene_->skybox()->bindActive(GL_TEXTURE0 + cubemapUnit_))
    {
        // We want to see the skybox texture from the inside
        gl->glCullFace(GL_FRONT);

        // When using depth texture, we don't want to use hardware z rejection
        if(gbuffer_ != nullptr)
        {
            gbuffer_->bindTextures();
            mesh_->render();
        }

        else
        {
            gl->glEnable(GL_DEPTH_TEST);
            gl->glDepthFunc(GL_LEQUAL);

            mesh_->render();

            gl->glDepthFunc(GL_LESS);
            gl->glDisable(GL_DEPTH_TEST);
        }

        gl->glCullFace(GL_BACK);
    }
}

void SkyboxStage::setRenderTarget(GLuint fbo)
{
    RenderStage::setRenderTarget(fbo);
    fbo_ = fbo;
}

void SkyboxStage::setSkyboxTechnique(const SkyboxPtr& skybox)
{
    skybox_ = skybox;
    initTechnique();
}

void SkyboxStage::setSkyboxMesh(const MeshPtr& mesh)
{
    mesh_ = mesh;
}

void SkyboxStage::initTechnique()
{
    if(skybox_ == nullptr)
    {
        return;
    }

    if(gbuffer_ != nullptr)
    {
        int depthUnit = gbuffer_->textures().indexOf("depth");
        cubemapUnit_ = depthUnit + 1;

        skybox_->setDepthTextureUnit(depthUnit);
    }

    skybox_->setTextureUnit(cubemapUnit_);
}