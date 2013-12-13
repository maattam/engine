#include "scene.h"

#include <cassert>

#include "entity/light.h"
#include "entity/camera.h"
#include "entity/frustum.h"

#include "renderer.h"
#include "visitor.h"
#include "renderqueue.h"

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

void Scene::addVisitor(BaseVisitor* visitor)
{
    visitors_.insert(visitor);
}

void Scene::removeVisitor(BaseVisitor* visitor)
{
    visitors_.remove(visitor);
}

void Scene::renderScene(Entity::Camera* camera)
{
    if(renderer_ != nullptr)
    {
        // Update scene transformations
        rootNode_.update();
        camera->update();

        renderer_->render(camera);
    }
}

void Scene::queryVisibles(const QMatrix4x4& viewProj, RenderQueue& renderQueue, bool shadowCasters)
{
    if(!shadowCasters)
    {
        lights_.clear();
    }

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
                         RenderQueue& queue, bool shadowCasters)
{
    if(node == nullptr)
    {
        return;
    }

    // Push entities from the node into the render queue
    if(node->numEntities() > 0)
    {
        const QMatrix4x4& nodeView = node->transformation();
        queue.setModelView(&nodeView);

        for(size_t i = 0; i < node->numEntities(); ++i)
        {
            // Skip entities that don't cast shadows
            if(shadowCasters && !node->isShadowCaster())
            {
                continue;
            }

            Entity::Entity* entity = node->getEntity(i);

            // Check whether the entity's bounding volume is inside our view frustrum
            if(isInsideFrustum(entity->boundingBox(), viewProj * nodeView))
            {
                // Notify observers. The observer can prevent the entity from being inserted to the render queue.
                if(notify(&SceneObserver::beforeRendering, entity, node))
                {
                    entity->updateRenderList(queue);
                }

                // Visit entity
                for(BaseVisitor* visitor : visitors_)
                {
                    entity->accept(*visitor);
                }
            }
        }
    }

    // Recursively walk through child nodes
    for(size_t i = 0; i < node->numChildren(); ++i)
    {
        Graph::SceneNode* inode = dynamic_cast<Graph::SceneNode*>(node->getChild(i));
        findVisibles(viewProj, inode, queue, shadowCasters);
    }
}