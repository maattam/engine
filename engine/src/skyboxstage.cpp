#include "skyboxstage.h"

#include "renderable/renderable.h"
#include "technique/skybox.h"
#include "cubemaptexture.h"
#include "graph/camera.h"
#include "gbuffer.h"
#include "binder.h"

using namespace Engine;

SkyboxStage::SkyboxStage(Renderer* renderer)
    : RenderStage(renderer), gbuffer_(nullptr), fbo_(0), cubemap_(nullptr), cubemapUnit_(0)
{
}

SkyboxStage::~SkyboxStage()
{
}

bool SkyboxStage::setViewport(const QRect& viewport, unsigned int samples)
{
    skybox_->setSampleCount(samples);
    return RenderStage::setViewport(viewport, samples);
}

void SkyboxStage::setGBuffer(GBuffer const* gbuffer)
{
    gbuffer_ = gbuffer;
    initTechnique();
}

void SkyboxStage::render(Graph::Camera* camera)
{
    RenderStage::render(camera);

    if(cubemap_ == nullptr || mesh_ == nullptr)
    {
        return; // Skybox not set
    }

    if(skybox_ == nullptr || !skybox_->enable())
    {
        return;
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    // Translate the skybox mesh to view origin
    QMatrix4x4 trans;
    trans.translate(camera->position());

    skybox_->setMVP(camera->worldView() * trans);

    if(Binder::bind(cubemap_, GL_TEXTURE0 + cubemapUnit_))
    {
        // We want to see the skybox texture from the inside
        gl->glCullFace(GL_FRONT);

        // When using depth texture, we don't want to use hardware z rejection
        if(gbuffer_ != nullptr)
        {
            // Enable blending to "resolve" the skybox based on MSAA sample visibility.
            gl->glEnable(GL_BLEND);
            gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            gbuffer_->bindTextures();
            mesh_->render();

            gl->glDisable(GL_BLEND);
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

void SkyboxStage::setSkyboxTexture(CubemapTexture* skybox)
{
    cubemap_ = skybox;
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