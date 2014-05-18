//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "forwardstage.h"
#include "gbuffer.h"
#include "resourcedespatcher.h"
#include "graph/camera.h"
#include "renderable/renderable.h"

using namespace Engine;

ForwardStage::ForwardStage(Renderer* renderer, ResourceDespatcher& despatcher)
    : RenderStage(renderer), batch_(nullptr), camera_(nullptr), fbo_(0)
{
    shader_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/forward.vert"), Shader::Type::Vertex));
    shader_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/forward.frag"), Shader::Type::Fragment));
}

ForwardStage::~ForwardStage()
{
}

void ForwardStage::setGeometryBatch(RenderQueue* batch)
{
    RenderStage::setGeometryBatch(batch);
    batch_ = batch;
}

void ForwardStage::setCamera(Graph::Camera* camera)
{
    RenderStage::setCamera(camera);
    camera_ = camera;
}

void ForwardStage::render()
{
    RenderStage::render();

    if(!shader_.enable())
    {
        return;
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gbuffer_->bindTextures();

    // TODO: Depth testing and back-to-front sorting
    renderRange(batch_->getItems(Material::RENDER_EMISSIVE));
    renderRange(batch_->getItems(Material::RENDER_TRANSPARENT));

    gl->glDisable(GL_BLEND);
}

void ForwardStage::setRenderTarget(GLuint fbo)
{
    RenderStage::setRenderTarget(fbo);
    fbo_ = fbo;
}

void ForwardStage::setGBuffer(GBuffer* gbuffer)
{
    gbuffer_ = gbuffer;

    int depthUnit = gbuffer_->textures().indexOf("depth");
    shader_.setDepthTextureUnit(depthUnit);
}

void ForwardStage::renderRange(const RenderQueue::RenderRange& range)
{
    for(auto it = range.first; it != range.second; ++it)
    {
        Material* material = it->material;
        Renderable::Renderable* renderable = it->renderable;

        shader_.setMVP(camera_->worldView() * *it->modelView);
        shader_.setMaterialAttributes(*material);

        if(material->bind())
        {
            renderable->render();
        }
    }
}