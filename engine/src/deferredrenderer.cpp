#include "deferredrenderer.h"

#include "common.h"
#include "resourcedespatcher.h"
#include "scene/visiblescene.h"
#include "renderable/renderable.h"
#include "entity/camera.h"

using namespace Engine;

DeferredRenderer::DeferredRenderer(ResourceDespatcher* despatcher)
    : errorMaterial_(despatcher), scene_(nullptr), camera_(nullptr)
{
    geometryShader_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/gbuffer.vert"), Shader::Type::Vertex));
    geometryShader_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/gbuffer.frag"), Shader::Type::Fragment));

    materialShader_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/dsmaterial.vert"), Shader::Type::Vertex));
    materialShader_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/dsmaterial.frag"), Shader::Type::Fragment));

    // Cache error material
    errorMaterial_.setTexture(Material::TEXTURE_DIFFUSE,
        despatcher->get<Texture2D>(RESOURCE_PATH("images/pink.png"), TC_SRGBA));
}

DeferredRenderer::~DeferredRenderer()
{
}

bool DeferredRenderer::setViewport(unsigned int width, unsigned int height, unsigned int samples,
                                   unsigned int left, unsigned int top)
{
    viewport_ = QRect(left, top, width, height);
    samples_ = samples;

    return initialise(width, height, samples);
}

void DeferredRenderer::setScene(VisibleScene* scene)
{
    scene_ = scene;
}

bool DeferredRenderer::setPostfxHook(Effect::Postfx* postfx)
{
    return false;
}

GBuffer* DeferredRenderer::getGBuffer()
{
    return &gbuffer_;
}

void DeferredRenderer::render(Entity::Camera* camera)
{
    camera_ = camera;

    // Cull visibles
    RenderQueue renderQueue;
    scene_->queryVisibles(camera->worldView(), renderQueue);

    gl->glViewport(viewport_.x(), viewport_.y(), viewport_.width(), viewport_.height());
    gl->glClearColor(0.0063f, 0.0063f, 0.0063f, 0);

    // Render scene geometry to GBuffer
    geometryPass(renderQueue);

    preLightPass();
}

void DeferredRenderer::geometryPass(const RenderQueue& queue)
{
    if(!geometryShader_.enable())
    {
        return;
    }

    gbuffer_.bindFbo();

    // Only the geometry pass can write to the gbuffer
    gl->glDepthMask(GL_TRUE);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl->glEnable(GL_DEPTH_TEST);

    for(auto it = queue.begin(); it != queue.end(); ++it)
    {
        Material* material = it->material;
        Renderable::Renderable* renderable = it->renderable;

        geometryShader_.setModelViewMatrix(*it->modelView);
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

void DeferredRenderer::preLightPass()
{
    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gl->glClear(GL_COLOR_BUFFER_BIT);

    // Output each GBuffer component to screen for now
    if(!materialShader_.enable())
    {
        return;
    }

    materialShader_.setProjMatrix(camera_->projection());

    gbuffer_.bindTextures();
    quad_.render();
}

bool DeferredRenderer::initialise(unsigned int width, unsigned int height, unsigned int samples)
{
    if(!gbuffer_.initialise(width, height, samples))
    {
        return false;
    }

    materialShader_.setSampleCount(samples);
    materialShader_.setDepthRange(0, 1);

    return true;
}

void DeferredRenderer::setFlags(unsigned int flags)
{
}

unsigned int DeferredRenderer::flags() const
{
    return 0;
}