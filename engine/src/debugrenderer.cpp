#include "debugrenderer.h"

#include "graph/camera.h"
#include "resourcedespatcher.h"
#include "graph/scenenode.h"
#include "graph/sceneleaf.h"
#include "graph/light.h"
#include "material.h"
#include "gbuffer.h"
#include "renderqueue.h"
#include "texture2d.h"
#include "renderable/primitive.h"

#include "scene/sceneobservable.h"

#include "binder.h"

using namespace Engine;

DebugRenderer::DebugRenderer(ResourceDespatcher* despatcher)
    : fbo_(0), batch_(nullptr), camera_(nullptr), flags_(0), gbuffer_(nullptr),
    quad_(Renderable::Primitive<Renderable::Quad>::instance()),
    boundingMesh_(Renderable::Primitive<Renderable::Cube>::instance())
{
    // AABB debugging tech
    aabbTech_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/aabb.vert"), Shader::Type::Vertex));
    aabbTech_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/aabb.frag"), Shader::Type::Fragment));

    // Wireframe tech
    wireframeTech_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/wireframe.vert"), Shader::Type::Vertex));
    wireframeTech_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/wireframe.frag"), Shader::Type::Fragment));

    // GBuffer visualizer
    gbufferMS_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/dsmaterial.vert"), Shader::Type::Vertex));

    ShaderData::DefineMap shaderDefines;
    shaderDefines.insert("SAMPLES", 1);

    gbufferMS_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/dsmaterial_debug.frag"), shaderDefines, Shader::Type::Fragment));
}

DebugRenderer::~DebugRenderer()
{
}

void DebugRenderer::setObservable(SceneObservable* observable)
{
    observable->addObserver(this);
}

void DebugRenderer::setCamera(Graph::Camera* camera)
{
    camera_ = camera;
}

bool DebugRenderer::setViewport(const QRect& viewport, unsigned int samples)
{
    viewport_ = viewport;

    return true;
}

void DebugRenderer::setRenderTarget(GLuint fbo)
{
    fbo_ = fbo;
}

void DebugRenderer::setGeometryBatch(RenderQueue* batch)
{
    batch_ = batch;
}

void DebugRenderer::setGBuffer(GBuffer const* gbuffer)
{
    gbuffer_ = gbuffer;
    gbufferMS_.setGBuffer(gbuffer_);
}

void DebugRenderer::render()
{
    // If no debug flags are set, bail out
    if(flags_ == 0)
    {
        return;
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    gl->glViewport(viewport_.x(), viewport_.y(), viewport_.width(), viewport_.height());

    if(flags_ != DEBUG_GBUFFER)
    {
        renderWireframe();
        renderAABBs();
    }

    renderGBuffer();
}

void DebugRenderer::renderWireframe()
{
    if(!(flags_ & DEBUG_WIREFRAME))
    {
        return;
    }

    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glClearColor(0.01f, 0.01f, 0.01f, 0);

    if(!wireframeTech_.bind())
    {
        return;
    }

    wireframeTech_->setUniformValue("gDiffuseSampler", 0);

    // Set polygon mode
    gl->glDisable(GL_CULL_FACE);
    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    RenderQueue::RenderRange range = batch_->getItems(Material::RENDER_OPAQUE);
    for(auto it = range.first; it != range.second; ++it)
    {
        wireframeTech_->setUniformValue("gMVP", camera_->worldView() * *it->modelView);

        Binder::bind(it->material->getTexture(Material::TEXTURE_DIFFUSE), GL_TEXTURE0);
            
        QVector3D highlight = it->material->attributes().ambientColor + QVector3D(0.2f, 0.2f, 0.2f);
        wireframeTech_->setUniformValue("ambientColor", highlight);
        wireframeTech_->setUniformValue("diffuseColor", it->material->attributes().diffuseColor);

        it->renderable->render();
    }

    // Reset polygon mode
    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    gl->glEnable(GL_CULL_FACE);
}

void DebugRenderer::renderAABBs()
{
    if(!aabbTech_.bind())
    {
        return;
    }

    gl->glDisable(GL_CULL_FACE);
    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for(auto it = aabbs_.begin(); it != aabbs_.end(); ++it)
    {
        aabbTech_->setUniformValue("gColor", it->second);
        aabbTech_->setUniformValue("gMVP", camera_->worldView() * it->first);
        boundingMesh_->render();
    }

    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    gl->glEnable(GL_CULL_FACE);

    aabbs_.clear();
}

void DebugRenderer::setFlags(unsigned int flags)
{
    flags_ = flags;
}

unsigned int DebugRenderer::flags() const
{
    return flags_;
}

bool DebugRenderer::beforeRendering(Graph::SceneLeaf* entity, Graph::SceneNode* node)
{
    if(flags_ & DEBUG_AABB)
    {
        QVector3D color(0, 0, 1);   // Mesh color is blue

        // Light volumes are green
        Graph::Light* light = dynamic_cast<Graph::Light*>(entity);
        if(light != nullptr)
        {
            color = QVector3D(0, 1, 0);
        }

        addAABB(node->transformation(), entity->boundingBox(), color);
    }

    return true;
}

void DebugRenderer::renderGBuffer()
{
    if(!(flags_ & DEBUG_GBUFFER) || gbuffer_ == nullptr)
    {
        return;
    }

    if(!gbufferMS_.enable())
    {
        return;
    }

    gbuffer_->bindTextures();
    gbufferMS_.setProjMatrix(camera_->projection());

    int width = viewport_.width() / 4;
    int height = viewport_.height() / 4;
    int numTextures = Technique::GBufferVisualizer::GB_COUNT;
    int gap = (viewport_.height() - numTextures * height) / (numTextures + 1);

    int viewY = gap;
    for(int i = 0; i < numTextures; ++i)
    {
        gl->glViewport(gap, viewY, width, height);
        gbufferMS_.setViewport(QRect(gap, viewY, width, height));

        // Render gbuffer texture
        gbufferMS_.outputTexture(static_cast<Technique::GBufferVisualizer::TextureType>(i));
        quad_->render();

        viewY += height + gap;
    }
}

void DebugRenderer::addAABB(const QMatrix4x4& trans, const AABB& aabb, const QVector3D& color)
{
    if(aabb.width() <= 0)
        return;

    QMatrix4x4 scale;
    scale.translate(aabb.center());
    scale.scale(0.5f);  // Our bounding box model is 2 units wide
    scale.scale(aabb.width(), aabb.height(), aabb.depth());   // Display over mesh surface

    aabbs_.push_back(std::make_pair(trans * scale, color));
}