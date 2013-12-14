#include "deferredrenderer.h"

#include <QOpenGLFramebufferObject>

#include "common.h"
#include "gbuffer.h"
#include "resourcedespatcher.h"
#include "scene/visiblescene.h"
#include "renderable/renderable.h"
#include "entity/camera.h"
#include "entity/light.h"
#include "graph/scenenode.h"
#include "renderqueue.h"

using namespace Engine;

DeferredRenderer::DeferredRenderer(GBuffer& gbuffer, ResourceDespatcher& despatcher)
    : gbuffer_(gbuffer), output_(nullptr), errorMaterial_(&despatcher), scene_(nullptr), camera_(nullptr)
{
    geometryShader_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/gbuffer.vert"), Shader::Type::Vertex));
    geometryShader_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/gbuffer.frag"), Shader::Type::Fragment));

    // Cache error material
    errorMaterial_.setTexture(Material::TEXTURE_DIFFUSE,
        despatcher.get<Texture2D>(RESOURCE_PATH("images/pink.png"), TC_SRGBA));
}

DeferredRenderer::~DeferredRenderer()
{
}

bool DeferredRenderer::setViewport(const QRect& viewport, unsigned int samples)
{
    viewport_ = viewport;

    return gbuffer_.initialise(viewport.width(), viewport.height(), samples);
}

void DeferredRenderer::setScene(VisibleScene* scene)
{
    scene_ = scene;
}

void DeferredRenderer::setOutputFBO(QOpenGLFramebufferObject* fbo)
{
    output_ = fbo;
}

void DeferredRenderer::render(Entity::Camera* camera)
{
    if(!gbuffer_.isInitialised())
    {
        return;
    }

    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_BACK);

    camera_ = camera;

    // Cull visibles
    renderQueue_.reset();
    scene_->queryVisibles(camera->worldView(), renderQueue_);

    gl->glViewport(viewport_.x(), viewport_.y(), viewport_.width(), viewport_.height());
    gl->glClearColor(0.0063f, 0.0063f, 0.0063f, 0);

    // Render scene geometry to GBuffer
    geometryPass();
}

void DeferredRenderer::geometryPass()
{
    if(!geometryShader_.enable())
    {
        return;
    }

    gbuffer_.bindFbo();

    // Only geometry pass can write to the gbuffer
    gl->glDepthMask(GL_TRUE);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl->glEnable(GL_DEPTH_TEST);

    QMatrix4x4 view = camera_->view();
    for(auto it = renderQueue_.begin(); it != renderQueue_.end(); ++it)
    {
        Material* material = it->material;
        Renderable::Renderable* renderable = it->renderable;

        geometryShader_.setNormalMatrix((view * *it->modelView).normalMatrix());
        geometryShader_.setMVP(camera_->worldView() * *it->modelView);

        if(!material->bind())
        {
            material = &errorMaterial_;     // Draw error material to indicate a problem with the object

            if(!errorMaterial_.bind())      // We can't fail further
                continue;
        }

        geometryShader_.setMaterialAttributes(material->attributes());
        geometryShader_.setHasTangentsAndNormals(renderable->hasTangents()
            && it->material->hasTexture(Material::TEXTURE_NORMALS));

        renderable->render();
    }

    gl->glDisable(GL_DEPTH_TEST);
    gl->glDepthMask(GL_FALSE);
}

void DeferredRenderer::setFlags(unsigned int flags)
{
}

unsigned int DeferredRenderer::flags() const
{
    return 0;
}