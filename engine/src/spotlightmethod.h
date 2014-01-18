//
//  Author   : Matti Määttä
//  Summary  : Shadow rendering method for spot lights.
//

#ifndef SPOTLIGHTMETHOD_H
#define SPOTLIGHTMETHOD_H

#include "shadowrendermethod.h"
#include "offscreenrenderer.h"
#include "technique/technique.h"

namespace Engine {

class SingleShadowMap;
class ResourceDespatcher;

class SpotLightMethod : public ShadowRenderMethod
{
public:
    SpotLightMethod(ResourceDespatcher& despatcher);
    virtual ~SpotLightMethod();

    virtual ShadowMap* createShadowMap();

    // It is assumed that ShadowMap contains enough textures required by the rendering method,
    // eg. point lights should have 6 textures where as spot lights only have 1 or cascades.
    // Precondition: shadow is same type as returned by createShadowMap.
    virtual void setShadowMap(ShadowMap* shadow);

    virtual void setCamera(Graph::Camera* camera);
    virtual void setSceneObservable(SceneObservable* observable);

    // Culls the scene through the light's perpective but does not render anything to fbo;
    // Precondition: Shadow map is set, observable is set, camera is set.
    virtual void prepare(Graph::Light& light);

    // Renders the scene from the light's perspective to the ShadowMap texture(s).
    // Precondition: Shadow map is set
    virtual void render();

private:
    SceneObservable* scene_;
    SingleShadowMap* shadow_;
    const QMatrix4x4* cachedVP_;

    OffscreenRenderer renderer_;
    Technique::Technique tech_;

    bool initTech_;
};

}

#endif