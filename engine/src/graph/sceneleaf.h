//
//  Author   : Matti Määttä
//  Summary  : SceneLeaf abstraction, which represents renderable geometry, lights, cameras etc.
//             in scene graph.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "aabb.h"
#include "visitable.h"

#include <QString>
#include <QVector>
#include <memory>

namespace Engine {

class RenderQueue;

namespace Graph {

class SceneNode;

class SceneLeaf : public BaseVisitable
{
public:
    SceneLeaf();
    virtual ~SceneLeaf();

    // Called by renderer; leaf must add its renderables to the list
    virtual void updateRenderList(RenderQueue& list) = 0;

    // Returns the axis-aligned bounding box which contains this Entity
    const AABB& boundingBox() const;

    // Attaches leaf to the given node.
    // precondition: node != nullptr
    virtual void attach(Graph::SceneNode* node);

    // Detaches the SceneLeaf from node
    virtual void detach();

    // Returns nullptr if the entity isn't attached to a node
    Graph::SceneNode* parentNode() const;

    void setName(const QString& name);
    const QString& name() const;

    virtual std::shared_ptr<SceneLeaf> clone() const = 0;

protected:
    void updateAABB(const AABB& aabb);

private:
    QString name_;
    AABB aabb_;

    Graph::SceneNode* node_;
};

}}

#endif //ENTITY_H