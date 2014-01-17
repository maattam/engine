//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "spotlightmethod.h"

#include "shadowmap.h"
#include "scene/sceneobservable.h"
#include "graph/light.h"
#include "graph/camera.h"
#include "graph/scenenode.h"
#include "graph/sceneleaf.h"
#include "mathelp.h"

using namespace Engine;

SpotLightMethod::SpotLightMethod()
    : shadow_(nullptr), scene_(nullptr)
{
    // TODO: Set technique and OnRenderCallback for renderer.
}

SpotLightMethod::~SpotLightMethod()
{
}

ShadowMap* SpotLightMethod::createShadowMap()
{
    // TODO: Initialise ShadowMap fbo
    return nullptr;
}

void SpotLightMethod::setShadowMap(ShadowMap* shadow)
{
    shadow_ = shadow;
}

void SpotLightMethod::setCamera(Graph::Camera* /*camera*/)
{
}

void SpotLightMethod::setSceneObservable(SceneObservable* observable)
{
    scene_ = observable;
}

void SpotLightMethod::prepare(Graph::Light& light)
{
    // Set up camera that fills the light's field of view.
    Graph::Camera lightView(1.0f, light.angleOuterCone() * 2);

    lightView.setNearPlane(0.1f);
    lightView.setFarPlane(light.cutoffDistance());
    lightView.setPosition(light.position());
    lightView.setOrientation(orientationFromAxes(
        QVector3D::crossProduct(light.direction(), UNIT_Y), UNIT_Y, light.direction()));

    lightView.update();

    // Query renderables inside the camera
    RenderQueue visibles;
    unsigned int lightMask = light.lightMask();

    scene_->findVisibleLeaves(lightView.worldView(), visibles,
        [lightMask] (const Graph::SceneLeaf&, const Graph::SceneNode& node)
        {
            // Accept only renderables that cast shadows by this light.
            return (lightMask & node.lightMask()) == lightMask;
        }
    );

    // TODO: Stuff visibles and camera in ShadowMap for future rendering.
}

void SpotLightMethod::render()
{
    // TODO: Get camera, visibles and fbo from ShadowMap.

    //renderer_.setCamera(camera);
    //renderer_.setGeometryBatch(visibles);

    //renderer_.setRenderTarget(fbo);

    renderer_.render();
}