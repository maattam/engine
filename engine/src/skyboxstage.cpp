#include "skyboxstage.h"

#include "renderable/renderable.h"
#include "technique/skybox.h"
#include "scene/visiblescene.h"
#include "cubemaptexture.h"
#include "entity/camera.h"
#include "gbuffer.h"

using namespace Engine;

SkyboxStage::SkyboxStage(Renderer* renderer)
    : RenderStage(renderer), gbuffer_(nullptr), fbo_(0), scene_(nullptr)
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

    if(skybox_ != nullptr)
    {
        skybox_->setDepthTextureUnit(gbuffer_->textures().indexOf("depth"));
    }
}

void SkyboxStage::render(Entity::Camera* camera)
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

    if(scene_->skybox()->bindActive(GL_TEXTURE0))
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

            gl->glDisable(GL_DEPTH_TEST);
        }

        gl->glCullFace(GL_BACK);
    }
}

void SkyboxStage::setOutputFBO(GLuint fbo)
{
    RenderStage::setOutputFBO(fbo);
    fbo_ = fbo;
}

void SkyboxStage::setSkyboxTechnique(const SkyboxPtr& skybox)
{
    skybox_ = skybox;
    skybox_->setTextureUnit(0);

    if(gbuffer_ != nullptr)
    {
        skybox_->setDepthTextureUnit(gbuffer_->textures().indexOf("depth"));
    }
}

void SkyboxStage::setSkyboxMesh(const MeshPtr& mesh)
{
    mesh_ = mesh;
}