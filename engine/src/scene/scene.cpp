#include "scene.h"

#include <cassert>

#include "graph/light.h"
#include "graph/camera.h"

#include "frustum.h"
#include "renderer.h"
#include "visitor.h"
#include "renderqueue.h"

using namespace Engine;

Scene::Scene() : camera_(nullptr), directionalLight_(nullptr)
{
}

Scene::~Scene()
{
}

void Scene::addVisitor(BaseVisitor* visitor)
{
    visitors_.insert(visitor);
}

void Scene::removeVisitor(BaseVisitor* visitor)
{
    visitors_.remove(visitor);
}

void Scene::setCamera(Graph::Camera* camera)
{
    camera_ = camera;
}

Graph::Camera* Scene::camera() const
{
    return camera_;
}

void Scene::update()
{
    rootNode_.update();
}

void Scene::queryVisibles(const QMatrix4x4& viewProj, RenderQueue& renderQueue, bool shadowCasters)
{
    findVisibles(viewProj, &rootNode_, renderQueue, shadowCasters);
}

Graph::Light* Scene::directionalLight() const
{
    return directionalLight_;
}

void Scene::setDirectionalLight(Graph::Light* light)
{
    if(light != nullptr && light->type() != Graph::Light::LIGHT_DIRECTIONAL)
    {
        return;
    }

    directionalLight_ = light;
}

CubemapTexture* Scene::skybox() const
{
    return skybox_.get();
}

void Scene::setSkybox(const std::shared_ptr<CubemapTexture>& texture)
{
    skybox_ = texture;
}

Graph::SceneNode* Scene::rootNode()
{
    return &rootNode_;
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

            Graph::SceneLeaf* entity = node->getEntity(i);

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
        Graph::SceneNode* inode = static_cast<Graph::SceneNode*>(node->getChild(i));
        findVisibles(viewProj, inode, queue, shadowCasters);
    }
}