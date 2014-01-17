//
//  Author   : Matti Määttä
//  Summary  : SceneManagers holds all SceneLeaves in the scene, performs culling of visible
//             geometry and rendering the scene through associated renderer.
//

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <memory>
#include <QRect>

namespace Engine {

namespace Graph {
    class SceneNode;
    class SceneLeaf;
}

class CubemapTexture;
class Renderer;

class SceneManager
{
public:
    typedef std::shared_ptr<Graph::SceneLeaf> SceneLeafPtr;
    typedef std::shared_ptr<CubemapTexture> SkyboxTexture;

    virtual ~SceneManager() {};

    // Renders the scene based on previously culled list of scene leaves.
    // Precondition: Renderer set
    virtual void renderFrame() = 0;

    // Culls visible scene leaves for rendering.
    virtual void prepareNextFrame() = 0;

    // Sets the renderer used to render the scene.
    // Precondition: renderer != nullptr
    virtual void setRenderer(Renderer* renderer) = 0;

    // Sets the output viewport. Changing viewport updates the camera's aspect ratio.
    virtual void setViewport(const QRect& viewport) = 0;

    // Sets the skybox texture.
    virtual void setSkyboxCubemap(const SkyboxTexture& sky) = 0;

    // Returns the root node of the scene. All leaves should be attached to this scene
    // so the manager can update the hierarchy when prepareNextFrame is called.
    virtual Graph::SceneNode& rootNode() = 0;

    // Adds a new scene leaf to the scene. If the leaf already exists, false is returned.
    // Postcondition: Ownership is maintained.
    virtual bool addSceneLeaf(const SceneLeafPtr& leaf) = 0;

    // Removes the leaf from scene. Returns false if leaf is not found.
    virtual bool removeSceneLeaf(const SceneLeafPtr& leaf) = 0;

    // Removes all scene leaves and clears the skybox texture.
    virtual void eraseScene() = 0;
};

}

#endif // SCENEMANAGER_H