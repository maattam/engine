#include "skyboxstage.h"

#include "renderable/renderable.h"
#include "technique/skybox.h"
#include "scene/visiblescene.h"
#include "cubemaptexture.h"
#include "entity/camera.h"
#include "gbuffer.h"

#include <QOpenGLFramebufferObject>

using namespace Engine;

SkyboxStage::SkyboxStage(Renderer* renderer)
    : RenderStage(renderer), gbuffer_(nullptr), fbo_(nullptr), scene_(nullptr)
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

    if(fbo_ == nullptr || !fbo_->bind())
    {
        gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    if(mesh_ == nullptr || skybox_ == nullptr || !skybox_->enable())
    {
        return;
    }

    // Translate the skybox mesh to view origin
    QMatrix4x4 trans;
    trans.translate(camera->position());
    skybox_->setMVP(camera->worldView() * trans);

    if(gbuffer_ != nullptr)
    {
        gbuffer_->bindTextures();
    }

    if(scene_->skybox()->bindActive(GL_TEXTURE0))
    {
        // We want to see the skybox texture from the inside
        gl->glCullFace(GL_FRONT);
        gl->glDepthFunc(GL_LEQUAL);

        mesh_->render();

        gl->glCullFace(GL_BACK);
    }
}

void SkyboxStage::setOutputFBO(QOpenGLFramebufferObject* fbo)
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