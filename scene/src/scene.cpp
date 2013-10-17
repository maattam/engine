#include "scene.h"

#include <cassert>

using namespace Engine;

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::cullVisibles(const Entity::Frustrum& frustrum, RenderQueue& renderQueue, bool shadowCasters)
{
    if(!shadowCasters)
    {
        lights_.clear();
    }

    renderQueue.clear();
    findVisibles(frustrum, &rootNode_, QMatrix4x4(), renderQueue, shadowCasters);
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

const CubemapTexture::Ptr& Scene::skybox() const
{
    return skybox_;
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

void Scene::findVisibles(const Entity::Frustrum& frustrum, Graph::SceneNode* node, const QMatrix4x4& worldView,
                  Scene::RenderQueue& queue, bool shadowCasters)
{
    if(node == nullptr)
    {
        return;
    }

    QMatrix4x4 nodeView = worldView * node->transformation();

    // Push entities from the node into the render queue
    if(node->numEntities() > 0)
    {
        VisibleNode visibleNode = std::make_pair(nodeView, RenderList());
        bool culled = false;

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
            if(frustrum.contains(entity->boundingBox(), nodeView))
            {
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
                    culled = true;
                }
            }
        }

        if(culled)
        {
            queue.push_back(visibleNode);
        }
    }

    // Recursively walk through child nodes
    for(size_t i = 0; i < node->numChildren(); ++i)
    {
        Graph::SceneNode* inode = dynamic_cast<Graph::SceneNode*>(node->getChild(i));
        findVisibles(frustrum, inode, nodeView, queue, shadowCasters);
    }
}