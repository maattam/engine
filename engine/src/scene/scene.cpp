#include "scene.h"

#include <cassert>

#include "entity/light.h"
#include "entity/camera.h"
#include "entity/frustum.h"

#include "renderer.h"

using namespace Engine;

Scene::Scene() : renderer_(nullptr), directionalLight_(nullptr)
{
}

Scene::~Scene()
{
}

void Scene::setView(Renderer* view)
{
    renderer_ = view;
}

void Scene::renderScene(Entity::Camera* camera)
{
    if(renderer_ != nullptr)
    {
        // Update scene transformations
        rootNode_.update();

        renderer_->render(camera, this);
    }
}

void Scene::queryVisibles(const QMatrix4x4& viewProj, RenderQueue& renderQueue, bool shadowCasters)
{
    if(!shadowCasters)
    {
        lights_.clear();
    }

    renderQueue.clear();

    findVisibles(viewProj, &rootNode_, renderQueue, shadowCasters);
}

Entity::Light* Scene::directionalLight() const
{
    return directionalLight_;
}

void Scene::setDirectionalLight(Entity::Light* light)
{
    if(light != nullptr && light->type() != Entity::Light::LIGHT_DIRECTIONAL)
    {
        return;
    }

    directionalLight_ = light;
}

CubemapTexture* Scene::skybox() const
{
    return skybox_.get();
}

void Scene::setSkybox(const CubemapTexture::Ptr& texture)
{
    skybox_ = texture;
}

Graph::SceneNode* Scene::rootNode()
{
    return &rootNode_;
}

const Scene::Lights& Scene::queryLights() const
{
    return lights_;
}

void Scene::findVisibles(const QMatrix4x4& viewProj, Graph::SceneNode* node,
                         Scene::RenderQueue& queue, bool shadowCasters)
{
    if(node == nullptr)
    {
        return;
    }

    // Push entities from the node into the render queue
    if(node->numEntities() > 0)
    {
        const QMatrix4x4& nodeView = node->transformation();
        VisibleNode visibleNode = std::make_pair(&nodeView, RenderList());

        for(size_t i = 0; i < node->numEntities(); ++i)
        {
            Entity::Entity* entity = node->getEntity(i);
            if(entity == nullptr)
            {
                continue;
            }
            
            // Skip entities that don't cast shadows
            if(shadowCasters && !node->isShadowCaster())
            {
                continue;
            }

            // Check whether the entity's bounding volume is inside our view frustrum
            if(isInsideFrustum(entity->boundingBox(), viewProj * nodeView))
            {
                // Notify observers
                notify(&SceneObserver::beforeRendering, entity, node);

                // Cache visible lights
                Entity::Light* light = dynamic_cast<Entity::Light*>(entity);
                if(!shadowCasters && light != nullptr)
                {
                    QVector3D position = nodeView.column(3).toVector3D();
                    lights_.push_back(std::make_pair(position, light));
                }

                else
                {
                    entity->updateRenderList(visibleNode.second);
                }
            }
        }

        // If some renderables were culled, push them down the queue
        if(visibleNode.second.size() > 0)
        {
            queue.push_back(visibleNode);
        }
    }

    // Recursively walk through child nodes
    for(size_t i = 0; i < node->numChildren(); ++i)
    {
        Graph::SceneNode* inode = dynamic_cast<Graph::SceneNode*>(node->getChild(i));
        findVisibles(viewProj, inode, queue, shadowCasters);
    }
}