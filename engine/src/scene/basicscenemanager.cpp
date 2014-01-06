#include "basicscenemanager.h"

#include "graph/sceneleaf.h"
#include "graph/camera.h"
#include "graph/light.h"
#include "frustum.h"
#include "renderer.h"
#include "cubemaptexture.h"

#include <QDebug>

using namespace Engine;

BasicSceneManager::BasicSceneManager()
    : renderer_(nullptr)
{
    addVisitor(this);
}

BasicSceneManager::~BasicSceneManager()
{
}

// Renders the scene based on previously culled list of scene leaves.
// Precondition: Renderer set
void BasicSceneManager::renderFrame()
{
    if(renderer_ == nullptr || culledCameras_.empty())
    {
        return;
    }

    renderer_->setSkyboxTexture(skybox_.get());
    renderer_->setGeometryBatch(&culledGeometry_);
    renderer_->setLights(culledLights_);

    // TODO: Handle multiple cameras
    renderer_->render(culledCameras_.first());
}

// Culls visible scene leaves for rendering.
void BasicSceneManager::prepareNextFrame()
{
    // If no cameras were previously culled, find all cameras in the scene
    if(culledCameras_.empty())
    {
        for(const SceneLeafPtr& leaf : leaves_)
        {
            leaf->accept(*this);
        }
    }

    culledGeometry_.clear();
    culledOccluders_.clear();
    culledLights_.clear();

    // If scene contains no cameras, there is nothing to cull
    if(culledCameras_.empty())
    {
        return;
    }

    rootNode_.propagate();

    Graph::Camera& camera = *culledCameras_.first();
    camera.setAspectRatio(static_cast<float>(viewport_.width()) / viewport_.height());
    camera.update();

    // Cull visible geometry and lights
    findVisibleLeaves(camera.worldView(), culledGeometry_);

    // Cull visibles inside each light's frustum
    for(Renderer::LightData& lightData : culledLights_)
    {
        Graph::Light* light = lightData.light;

        if(light->lightMask() & Graph::Light::MASK_CAST_SHADOWS)
        {
            Occluders occluders;
            occluders.light = lightData.light;
            occluders.geometry.reset(new RenderQueue());

            culledOccluders_.push_back(occluders);

            // TODO: Move Light::frustrum to separate class.
            findOccluders(light->lightMask(), light->frustum(), *occluders.geometry);

            lightData.occluders = occluders.geometry.get();
        }
    }
}

void BasicSceneManager::findVisibleLeaves(const QMatrix4x4& viewProj, RenderQueue& queue)
{
    for(const SceneLeafPtr& leaf : leaves_)
    {
        Graph::SceneNode* node = leaf->parentNode();

        // Skip nodes that are not attached to scenegraph
        if(node == nullptr)
        {
            continue;
        }

        queue.setModelView(&node->transformation());
        if(isInsideFrustum(leaf->boundingBox(), viewProj * node->transformation()))
        {
            if(notify(&SceneObserver::beforeRendering, leaf.get(), node))
            {
                leaf->updateRenderList(queue);
            }

            for(BaseVisitor* visitor : visitors_)
            {
                leaf->accept(*visitor);
            }
        }
    }
}

void BasicSceneManager::findOccluders(unsigned int lightMask, const QMatrix4x4& frustum, RenderQueue& queue)
{
    for(const SceneLeafPtr& leaf : leaves_)
    {
        Graph::SceneNode* node = leaf->parentNode();

        // Skip nodes that are not attached to scenegraph, or don't cast shadows
        if(node == nullptr || node->lightMask() & lightMask == 0)
        {
            continue;
        }

        queue.setModelView(&node->transformation());
        if(isInsideFrustum(leaf->boundingBox(), frustum * node->transformation()))
        {
            leaf->updateRenderList(queue);
        }
    }
}

// Sets the renderer used to render the scene.
// Precondition: renderer != nullptr
void BasicSceneManager::setRenderer(Renderer* renderer)
{
    renderer_ = renderer;
}

// Sets the output viewport. Changing viewport updates the camera's aspect ratio
// and renderers viewport.
void BasicSceneManager::setViewport(const QRect& viewport)
{
    viewport_ = viewport;
}

// Sets the skybox texture.
void BasicSceneManager::setSkyboxCubemap(const SkyboxTexture& sky)
{
    skybox_ = sky;
}

// Returns the root node of the scene. All leaves should be attached to this scene
// so the manager can update the hierarchy when prepareNextFrame is called.
// Postcondition: rootNode != nullptr
Graph::SceneNode& BasicSceneManager::rootNode()
{
    return rootNode_;
}

// Adds a new scene leaf to the scene. If the leaf already exists, false is returned.
// Postcondition: Ownership is maintained.
bool BasicSceneManager::addSceneLeaf(const SceneLeafPtr& leaf)
{
    if(leaves_.count(leaf) > 0)
    {
        return false;
    }

    leaves_.push_back(leaf);
    return true;
}

// Removes the leaf from scene. Returns false if leaf is not found.
bool BasicSceneManager::removeSceneLeaf(const SceneLeafPtr& leaf)
{
    int index = leaves_.indexOf(leaf);
    if(index == -1)
    {
        return false;
    }

    for(auto it = culledCameras_.begin(); it != culledCameras_.end(); ++it)
    {
        if(*it == leaves_[index].get())
        {
            culledCameras_.erase(it);
            break;
        }
    }

    leaves_.remove(index);
    return true;
}

// Removes all scene leaves and clears the skybox texture.
void BasicSceneManager::eraseScene()
{
    culledCameras_.clear();
    leaves_.clear();
    setSkyboxCubemap(nullptr);
}

// Adds a scene leaf visitor, which will be called for culled leaves.
// If the visitor already exists, it won't be duplicated.
// precondition: visitor != nullptr
void BasicSceneManager::addVisitor(BaseVisitor* visitor)
{
    visitors_.insert(visitor);
}

// Removes a visitor from the visitor list.
void BasicSceneManager::removeVisitor(BaseVisitor* visitor)
{
    visitors_.remove(visitor);
}

void BasicSceneManager::visit(Graph::Light& light)
{
    Renderer::LightData lightData;
    lightData.light = &light;

    culledLights_.push_back(lightData);
}

void BasicSceneManager::visit(Graph::Camera& camera)
{
    if(culledCameras_.indexOf(&camera) == -1)
    {
        culledCameras_.push_back(&camera);
    }
}