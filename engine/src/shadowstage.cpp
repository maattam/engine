#include "shadowstage.h"

#include "scene/sceneobservable.h"
#include "shadowrendermethod.h"
#include "shadowmap.h"

using namespace Engine;

ShadowStage::ShadowStage(Renderer* renderer)
    : RenderStage(renderer), SceneObserver(), BaseVisitor(), observable_(nullptr)
{
}

ShadowStage::~ShadowStage()
{
    if(observable_ != nullptr)
    {
        observable_->removeVisitor(this);
    }
}

void ShadowStage::setMethod(Graph::Light::LightType type, ShadowMethodPtr method)
{
    methods_[type] = method;

    if(method != nullptr)
    {
        method->setSceneObservable(observable_);
    }
}

bool ShadowStage::createShadowMap(Graph::Light::LightType type, const QSize& size, unsigned int count)
{
    QVector<ShadowMapPtr>& maps = shadowMaps_[type];

    // Rebuild all maps
    maps.clear();
    maps.resize(count);

    const ShadowMethodPtr& method = methods_[type];
    if(method == nullptr)
    {
        return false;
    }

    for(ShadowMapPtr& map : maps)
    {
        map.reset(method->createShadowMap());
        map->setSize(size);

        if(!map->create())
        {
            return false;
        }
    }

    return true;
}

ShadowMap* ShadowStage::shadowMap(Graph::Light* light) const
{
    ShadowMap* map = nullptr;

    // Return cached shadow map
    auto result = lightIndices_.find(light);
    if(result != lightIndices_.end())
    {
        map = result.value();
    }

    return map;
}

void ShadowStage::setObservable(SceneObservable* observable)
{
    RenderStage::setObservable(observable);
    observable_ = observable;

    for(int i = 0; i < Graph::Light::LIGHT_COUNT; ++i)
    {
        if(methods_[i] != nullptr)
        {
            methods_[i]->setSceneObservable(observable_);
        }
    }
}

void ShadowStage::setCamera(Graph::Camera* camera)
{
    RenderStage::setCamera(camera);

    for(int i = 0; i < Graph::Light::LIGHT_COUNT; ++i)
    {
        if(methods_[i] != nullptr)
        {
            methods_[i]->setCamera(camera);
        }
    }
}

void ShadowStage::render()
{
    for(auto it = lightIndices_.begin(); it != lightIndices_.end(); ++it)
    {
        const ShadowMethodPtr& method = methods_[it.key()->type()];

        method->setShadowMap(it.value());
        method->render();
    }

    RenderStage::render();
}

void ShadowStage::visit(Graph::Light& light)
{
    if(light.lightMask() & Graph::Light::MASK_CAST_SHADOWS)
    {
        const ShadowMethodPtr& method = methods_[light.type()];
        ShadowMap* map = availableShadowMap(light.type());

        // Prepare light shadow map for rendering
        if(method != nullptr && map != nullptr)
        {
            method->setShadowMap(map);
            method->prepare(light);

            lightIndices_.insert(&light, map);
        }
    }
}

void ShadowStage::sceneInvalidated()
{
    lightIndices_.clear();

    // Reset free list to beginning.
    // Immutable shadow maps (eg. directional light) could be marked here for optimisation.
    for(int i = 0; i < Graph::Light::LIGHT_COUNT; ++i)
    {
        freeMaps_[i] = shadowMaps_[i].begin();
    }
}

ShadowMap* ShadowStage::availableShadowMap(Graph::Light::LightType type) const
{
    ShadowMap* map = nullptr;

    ShadowMapVec::iterator iter = freeMaps_[type];
    if(iter != shadowMaps_[type].end())
    {
        map = iter->get();
        ++iter;
    }

    return map;
}