#include "deferredrenderer.h"

#include "common.h"
#include "gbuffer.h"
#include "resourcedespatcher.h"
#include "renderable/renderable.h"
#include "graph/camera.h"
#include "renderqueue.h"
#include "texture2dresource.h"

using namespace Engine;

DeferredRenderer::DeferredRenderer(const GBufferPtr& gbuffer, ResourceDespatcher& despatcher)
    : gbuffer_(gbuffer), renderQueue_(nullptr), camera_(nullptr)
{
    geometryShader_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/gbuffer.vert"), Shader::Type::Vertex));
    geometryShader_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/gbuffer.frag"), Shader::Type::Fragment));

    // Cache error material
    errorMaterial_.setTexture(Material::TEXTURE_DIFFUSE,
        despatcher.get<Texture2DResource>(RESOURCE_PATH("images/pink.png"), TC_SRGBA));
}

DeferredRenderer::~DeferredRenderer()
{
}

bool DeferredRenderer::setViewport(const QRect& viewport, unsigned int samples)
{
    if(gbuffer_ == nullptr)
    {
        return false;
    }

    viewport_ = viewport;
    return gbuffer_->initialise(viewport.width(), viewport.height(), samples);
}

void DeferredRenderer::setGeometryBatch(RenderQueue* batch)
{
    renderQueue_ = batch;
}

void DeferredRenderer::setRenderTarget(GLuint /*fbo*/)
{
    // Geometry pass doesn't output anything to screen
}

void DeferredRenderer::setLights(const QVector<LightData>& /*lights*/)
{
    // Lightning is accumulated in another RenderStage
}

void DeferredRenderer::setSkyboxTexture(CubemapTexture* /*skybox*/)
{
}

void DeferredRenderer::render(Graph::Camera* camera)
{
    if(!gbuffer_->isInitialised())
    {
        return;
    }

    camera_ = camera;

    // Cull visibles
    gl->glViewport(viewport_.x(), viewport_.y(), viewport_.width(), viewport_.height());

    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_CULL_FACE);

    // Render scene geometry to GBuffer
    geometryPass();

    gl->glDisable(GL_CULL_FACE);
    gl->glDisable(GL_DEPTH_TEST);
}

void DeferredRenderer::geometryPass()
{
    if(!geometryShader_.enable())
    {
        return;
    }

    gbuffer_->bindFbo();

    gl->glClearColor(0, 0, 0, 0);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const QMatrix4x4 view = camera_->view();

    // Render only opaque items to gbuffer
    RenderQueue::RenderRange range = renderQueue_->getItems(Material::RENDER_OPAQUE);
    for(auto it = range.first; it != range.second; ++it)
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
            && material->hasTexture(Material::TEXTURE_NORMALS));

        renderable->render();
    }
}