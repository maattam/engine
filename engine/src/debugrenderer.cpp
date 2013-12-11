#include "debugrenderer.h"

#include "entity/camera.h"
#include "resourcedespatcher.h"
#include "graph/scenenode.h"
#include "entity/entity.h"
#include "entity/light.h"
#include "material.h"
#include "gbuffer.h"

using namespace Engine;

DebugRenderer::DebugRenderer(ResourceDespatcher* despatcher)
    : observable_(nullptr), scene_(nullptr), camera_(nullptr), flags_(0), gbuffer_(nullptr)
{
    // AABB debugging tech
    aabbTech_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/aabb.vert"), Shader::Type::Vertex));
    aabbTech_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/aabb.frag"), Shader::Type::Fragment));

    // Wireframe tech
    wireframeTech_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/wireframe.vert"), Shader::Type::Vertex));
    wireframeTech_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/wireframe.frag"), Shader::Type::Fragment));

    // GBuffer visualizer
    gbufferMS_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/dsmaterial.vert"), Shader::Type::Vertex));
    gbufferMS_.addShader(despatcher->get<Shader>(RESOURCE_PATH("shaders/dsmaterial_debug.frag"), Shader::Type::Fragment));
}

DebugRenderer::~DebugRenderer()
{
}

bool DebugRenderer::setViewport(unsigned int width, unsigned int height, unsigned int samples,
                 unsigned int left, unsigned int top)
{
    viewport_ = QRect(left, top, width, height);

    gbufferMS_.setSampleCount(samples);
    gbufferMS_.setDepthRange(0, 1);

    return true;
}

bool DebugRenderer::setPostfxHook(Effect::Postfx* postfx)
{
    return false;
}

void DebugRenderer::setScene(VisibleScene* scene)
{
    scene_ = scene;
}

void DebugRenderer::setGBuffer(GBuffer* gbuffer)
{
    gbuffer_ = gbuffer;
}

void DebugRenderer::render(Entity::Camera* camera)
{
    camera_ = camera;

    // If no debug flags are set, bail out
    if(observable_ == nullptr || flags_ == 0)
    {
        return;
    }

    observable_->addObserver(this);

    // Cull visibles
    RenderQueue renderQueue;
    scene_->queryVisibles(camera->worldView(), renderQueue);

    gl->glViewport(viewport_.x(), viewport_.y(), viewport_.width(), viewport_.height());

    renderWireframe(renderQueue);
    renderAABBs();
    renderGBuffer();

    observable_->removeObserver(this);
}

void DebugRenderer::renderWireframe(const RenderQueue& queue)
{
    if(!(flags_ & DEBUG_WIREFRAME))
    {
        return;
    }

    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glClearColor(0.0063f, 0.0063f, 0.0063f, 0);

    if(!wireframeTech_.bind())
    {
        return;
    }

    wireframeTech_->setUniformValue("gDiffuseSampler", 0);

    // Set polygon mode
    gl->glDisable(GL_CULL_FACE);
    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for(auto it = queue.begin(); it != queue.end(); ++it)
    {
        wireframeTech_->setUniformValue("gMVP", camera_->worldView() * *it->modelView);

        it->material->getTexture(Material::TEXTURE_DIFFUSE)->bindActive(GL_TEXTURE0);
            
        QVector3D highlight = it->material->attributes().ambientColor + QVector3D(0.2, 0.2, 0.2);
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
        boundingMesh_.render();
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

void DebugRenderer::setObservable(ObservableType* obs)
{
    observable_ = obs;
}

void DebugRenderer::beforeRendering(Entity::Entity* entity, Graph::SceneNode* node)
{
    if(flags_ & DEBUG_AABB)
    {
        QVector3D color(0, 0, 1);   // Mesh color is blue

        // Light volumes are green
        if(dynamic_cast<Entity::Light*>(entity) != nullptr)
        {
            color = QVector3D(0, 1, 0);
        }

        addAABB(node->transformation(), entity->boundingBox(), color);
    }
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
        gl->glViewport(16, viewY, width, height);

        // Render gbuffer texture
        gbufferMS_.outputTexture(static_cast<Technique::GBufferVisualizer::TextureType>(i));
        quad_.render();

        viewY += height + gap;
    }
}

void DebugRenderer::addAABB(const QMatrix4x4& trans, const Entity::AABB& aabb, const QVector3D& color)
{
    if(aabb.width() <= 0)
        return;

    QMatrix4x4 scale;
    scale.translate(aabb.center());
    scale.scale(0.5f);  // Our bounding box model is 2 units wide
    scale.scale(aabb.width(), aabb.height(), aabb.depth());   // Display over mesh surface

    aabbs_.push_back(std::make_pair(trans * scale, color));
}