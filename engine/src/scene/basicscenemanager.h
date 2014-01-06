// Basic scene manager which doesn't rely on any spatial structure to organise culling.
// Inefficient for complex scenes.

#ifndef BASICSCENEMANAGER_H
#define BASICSCENEMANAGER_H

#include "scenemanager.h"
#include "visitor.h"
#include "graph/scenenode.h"
#include "renderqueue.h"
#include "renderer.h"

#include <QVector>
#include <QSet>

namespace Engine {

namespace Graph {
    class Light;
    class Camera;
}

class BasicSceneManager : public SceneManager,
    public BaseVisitor, public Visitor<Graph::Light>, public Visitor<Graph::Camera>
{
public:
    BasicSceneManager();
    virtual ~BasicSceneManager();

    // Renders the scene based on previously culled list of scene leaves.
    // Precondition: Renderer set
    virtual void renderFrame();

    // Culls visible scene leaves for rendering.
    virtual void prepareNextFrame();

    // Sets the renderer used to render the scene.
    // Precondition: renderer != nullptr
    virtual void setRenderer(Renderer* renderer);

    // Sets the output viewport. Changing viewport updates the camera's aspect ratio
    // and renderers viewport.
    virtual void setViewport(const QRect& viewport);

    // Sets the skybox texture.
    virtual void setSkyboxCubemap(const SkyboxTexture& sky);

    // Returns the root node of the scene. All leaves should be attached to this scene
    // so the manager can update the hierarchy when prepareNextFrame is called.
    // Postcondition: rootNode != nullptr
    virtual Graph::SceneNode& rootNode();

    // Adds a new scene leaf to the scene. If the leaf already exists, false is returned.
    // Postcondition: Ownership is maintained.
    virtual bool addSceneLeaf(const SceneLeafPtr& leaf);

    // Removes the leaf from scene. Returns false if leaf is not found.
    virtual bool removeSceneLeaf(const SceneLeafPtr& leaf);

    // Removes all scene leaves and clears the skybox texture.
    virtual void eraseScene();

    // Adds a scene leaf visitor, which will be called for culled leaves.
    // If the visitor already exists, it won't be duplicated.
    // precondition: visitor != nullptr
    virtual void addVisitor(BaseVisitor* visitor);

    // Removes a visitor from the visitor list.
    virtual void removeVisitor(BaseVisitor* visitor);

    virtual void visit(Graph::Light& light);
    virtual void visit(Graph::Camera& camera);

protected:
    virtual void findVisibleLeaves(const QMatrix4x4& viewProj, RenderQueue& queue);
    virtual void findOccluders(unsigned int lightMask, const QMatrix4x4& frustum, RenderQueue& queue);

private:
    Renderer* renderer_;

    Graph::SceneNode rootNode_;
    SkyboxTexture skybox_;
    QRect viewport_;

    QVector<SceneLeafPtr> leaves_;
    QSet<BaseVisitor*> visitors_;

    QVector<Renderer::LightData> culledLights_;
    QVector<Graph::Camera*> culledCameras_;

    struct Occluders
    {
        Graph::Light* light;
        std::shared_ptr<RenderQueue> geometry;
    };

    QVector<Occluders> culledOccluders_;
    RenderQueue culledGeometry_;

    BasicSceneManager(const BasicSceneManager&);
    BasicSceneManager& operator=(const BasicSceneManager&);
};

}

#endif // BASICSCENEMANAGER_H