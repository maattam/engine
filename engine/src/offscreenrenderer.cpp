#include "offscreenrenderer.h"

#include "material.h"
#include "technique/technique.h"
#include "renderable/renderable.h"
#include "graph/camera.h"

using namespace Engine;

OffscreenRenderer::OffscreenRenderer()
    : Renderer(), batch_(nullptr), camera_(nullptr), tech_(nullptr),
    fbo_(0), renderCallback_(nullptr)
{
}

OffscreenRenderer::~OffscreenRenderer()
{
}

void OffscreenRenderer::setObservable(SceneObservable* /*observable*/)
{
}

bool OffscreenRenderer::setViewport(const QRect& viewport, unsigned int /*samples*/)
{
    viewport_ = viewport;
    return true;
}

void OffscreenRenderer::setGeometryBatch(RenderQueue* batch)
{
    batch_ = batch;
}

void OffscreenRenderer::setCamera(Graph::Camera* camera)
{
    camera_ = camera;
}

void OffscreenRenderer::setRenderTarget(GLuint fbo)
{
    fbo_ = fbo;
}

void OffscreenRenderer::setTechnique(Technique::Technique* tech)
{
    tech_ = tech;
}

void OffscreenRenderer::setRenderCallback(OnRenderCallback callback)
{
    renderCallback_ = callback;
}

void OffscreenRenderer::render()
{
    if(batch_ == nullptr || camera_ == nullptr || tech_ == nullptr)
    {
        return;
    }

    // Bind shader
    if(!tech_->enable())
    {
        return;
    }

    // Bind framebuffer
    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl->glViewport(viewport_.x(), viewport_.y(), viewport_.width(), viewport_.height());

    for(int i = 0; i < Material::RENDER_COUNT; ++i)
    {
        Material::RenderType index = static_cast<Material::RenderType>(i);
        renderBatch(batch_->getItems(index));
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OffscreenRenderer::renderBatch(const RenderQueue::RenderRange& range)
{
    const QMatrix4x4& worldView = camera_->worldView();

    for(auto it = range.first; it != range.second; ++it)
    {
        if(renderCallback_ != nullptr)
        {
            renderCallback_(*it->material, worldView * *it->modelView);
        }

        it->renderable->render();
    }
}