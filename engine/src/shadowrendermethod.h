#ifndef SHADOWRENDERMETHOD_H
#define SHADOWRENDERMETHOD_H

namespace Engine {

class ShadowMap;
class SceneObservable;

namespace Graph {
    class Camera;
    class Light;
}

class ShadowRenderMethod
{
public:
    virtual ~ShadowRenderMethod() {}

    virtual ShadowMap* createShadowMap() = 0;

    // It is assumed that ShadowMap contains enough textures required by the rendering method,
    // eg. point lights should have 6 textures where as spot lights only have 1 or cascades.
    // Precondition: shadow is same type as returned by createShadowMap.
    virtual void setShadowMap(ShadowMap* shadow) = 0;

    virtual void setCamera(Graph::Camera* camera) = 0;
    virtual void setSceneObservable(SceneObservable* observable) = 0;

    // Culls the scene through the light's perpective but does not render anything to fbo;
    // Precondition: Shadow map is set, observable is set, camera is set.
    virtual void prepare(Graph::Light& light) = 0;

    // Renders the scene from the light's perspective to the ShadowMap texture(s).
    // Precondition: Shadow map is set
    virtual void render() = 0;
};

}

#endif // SHADOWRENDERMETHOD_H