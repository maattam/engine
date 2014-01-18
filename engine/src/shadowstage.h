//
//  Author   : Matti Määttä
//  Summary  : ShadowStage listens for visible lights in the scene and renders light
//             shadow textures (maps). ShadowStage should be attached to the (first)
//             lightning stage that uses shadows.
//

#ifndef SHADOWSTAGE_H
#define SHADOWSTAGE_H

#include "renderstage.h"
#include "visitor.h"
#include "scene/sceneobserver.h"

#include "graph/light.h"

#include <QVector>
#include <QMap>
#include <QSize>
#include <memory>

namespace Engine {

class ShadowRenderMethod;
class ShadowMap;

class ShadowStage : public RenderStage, public SceneObserver,
    public BaseVisitor, public Visitor<Graph::Light>
{
public:
    explicit ShadowStage(Renderer* renderer);
    virtual ~ShadowStage();

    typedef std::shared_ptr<ShadowRenderMethod> ShadowMethodPtr;

    // Associates a method with the given light type. Existing methods are replaced with the new one.
    void setMethod(Graph::Light::LightType type, ShadowMethodPtr method);

    // Sets the maximum amount of visible shadow casting lights per type and shadow map resolution.
    bool createShadowMap(Graph::Light::LightType type, const QSize& size, unsigned int count);

    // Returns the shadow map associated with the light.
    // Returns nullptr if no shadow map is associated with the light.
    ShadowMap* shadowMap(Graph::Light* light) const;

    // Sets the observable for the current scene.
    // precondition: observable != nullptr.
    virtual void setObservable(SceneObservable* observable);

    // Sets the camera for the current geometry batch.
    // precondition: camera != nullptr
    virtual void setCamera(Graph::Camera* camera);

    // Renders the scene through the camera's viewport.
    // preconditions: scene has been set, viewport has been set, camera != nullptr
    virtual void render();

    virtual void visit(Graph::Light& light);

    virtual void sceneInvalidated();

private:
    SceneObservable* observable_;

    typedef std::shared_ptr<ShadowMap> ShadowMapPtr;
    typedef QVector<ShadowMapPtr> ShadowMapVec;

    // Associate shadow rendering method with each light type.
    ShadowMethodPtr methods_[Graph::Light::LIGHT_COUNT];

    ShadowMapVec shadowMaps_[Graph::Light::LIGHT_COUNT];
    ShadowMapVec::iterator freeMaps_[Graph::Light::LIGHT_COUNT];

    QMap<Graph::Light*, ShadowMap*> lightIndices_;

    ShadowMap* availableShadowMap(Graph::Light::LightType type);
};

}

#endif // SHADOWSTAGE_H