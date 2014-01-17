//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "forwardrenderer.h"

#include <QMatrix4x4>
#include <QDebug>

#include "graph/camera.h"
#include "graph/light.h"
#include "graph/scenenode.h"
#include "renderable/renderable.h"
#include "resourcedespatcher.h"
#include "texture2dresource.h"
#include "scene/sceneobservable.h"

using namespace Engine;

ForwardRenderer::ForwardRenderer(ResourceDespatcher& despatcher)
    : renderQueue_(nullptr), samples_(1), fbo_(0), camera_(nullptr), directionalLight_(nullptr), observable_(nullptr)
{
    // Cache error material
    errorMaterial_.setTexture(Material::TEXTURE_DIFFUSE,
        despatcher.get<Texture2DResource>(RESOURCE_PATH("images/pink.png"), TC_SRGBA));

    // BasicLightning shaders
    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/basiclightning.vert"), Shader::Type::Vertex));
    lightningTech_.addShader(despatcher.get<Shader>(RESOURCE_PATH("shaders/basiclightning.frag"), Shader::Type::Fragment));
}

ForwardRenderer::~ForwardRenderer()
{
    if(observable_ != nullptr)
    {
        // TODO: Detach visitors automatically
        observable_->removeVisitor(this);
    }
}

void ForwardRenderer::setObservable(SceneObservable* observable)
{
    observable->addVisitor(this);
    observable->addObserver(this);

    observable_ = observable;
}

void ForwardRenderer::setCamera(Graph::Camera* camera)
{
    camera_ = camera;
}

void ForwardRenderer::sceneInvalidated()
{
    lights_.clear();
    directionalLight_ = nullptr;
}

bool ForwardRenderer::setViewport(const QRect& viewport, unsigned int samples)
{
    viewport_ = viewport;
    samples_ = samples;

    return true;
}


void ForwardRenderer::setGeometryBatch(RenderQueue* batch)
{
    renderQueue_ = batch;
}

void ForwardRenderer::visit(Graph::Light& light)
{
    if(light.type() == Graph::Light::LIGHT_DIRECTIONAL)
    {
        directionalLight_ = &light;
    }

    else
    {
        lights_.push_back(&light);
    }
}

void ForwardRenderer::setRenderTarget(GLuint fbo)
{
    fbo_ = fbo;
}

void ForwardRenderer::render()
{
    gl->glEnable(GL_CULL_FACE);
    gl->glEnable(GL_DEPTH_TEST);

    // Render geometry and lightning
    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    renderPass();

    gl->glDisable(GL_DEPTH_TEST);
    gl->glDisable(GL_CULL_FACE);
}

void ForwardRenderer::renderPass()
{
    // Prepare OpenGL state for render pass
    gl->glViewport(viewport_.x(), viewport_.y(), viewport_.width(), viewport_.height());
    gl->glClearColor(0.0063f, 0.0063f, 0.0063f, 0);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!lightningTech_.enable())
    {
        return;
    }

    lightningTech_.setEyeWorldPos(camera_->position());
    lightningTech_.setDirectionalLight(directionalLight_);

    lightningTech_.setPointAndSpotLights(lights_);
    lightningTech_.setShadowEnabled(false);

    // Render opaque and emissive objects
    renderRange(renderQueue_->getItems(Material::RENDER_OPAQUE));
    renderRange(renderQueue_->getItems(Material::RENDER_EMISSIVE));
}

void ForwardRenderer::renderRange(RenderQueue::RenderRange range)
{
    for(auto it = range.first; it != range.second; ++it)
    {
        lightningTech_.setWorldView(*it->modelView);
        lightningTech_.setMVP(camera_->worldView() * *it->modelView);

        renderNode(*it);
    }
}

void ForwardRenderer::renderNode(const RenderQueue::RenderItem& node)
{
    Material* material = node.material;
    Renderable::Renderable* renderable = node.renderable;

    if(!material->bind())
    {
        material = &errorMaterial_;     // Draw error material to indicate a problem with the object

        if(!errorMaterial_.bind())      // We can't fail further
            return;
    }

    lightningTech_.setHasTangents(renderable->hasTangents() && material->hasTexture(Material::TEXTURE_NORMALS));
    lightningTech_.setMaterialAttributes(material->attributes());

    renderable->render();
}