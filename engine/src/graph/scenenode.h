//
//  Author   : Matti Määttä
//  Summary  : Scene graph node which represents hierarchial rotation, scale and translation relative
//             to child nodes.
//

#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector>

namespace Engine { namespace Graph {

class SceneNode
{
public:
    typedef QVector<SceneNode*> ChildSceneNodes;

    SceneNode();
    virtual ~SceneNode();

    // Returns nullptr the node is root of the hierarchy
    SceneNode* getParent() const;

    // Caches the node's world transformation if the orientation, scale or
    // translation has changed.
    // If the node has children, the children's transformations are cached recursively.
    void propagate();

    // Returns the cached world transformation of this node.
    const QMatrix4x4& transformation() const;

    // Applies given transformation matrix to this node.
    void applyTransformation(const QMatrix4x4& matrix);

    // Set node position in world space
    void setPosition(const QVector3D& position);
    QVector3D position() const;
    QVector3D worldPosition() const;

    // Adds offset to the node's position
    void move(const QVector3D& offset);

    // Rotates the node's orientation by quaternion
    void rotate(const QQuaternion& quaternion);

    // Rotates the node's orientation by angle degrees along given axis
    void rotate(float angle, const QVector3D& axis);

    void setOrientation(const QQuaternion& quaternion);
    QQuaternion orientation() const;
    QQuaternion worldOrientation() const;

    // Makes the node face the given direction.
    void setDirection(const QVector3D& direction);

    // Returns the direction the node is facing
    QVector3D direction() const;

    // Helper function that sets the node to face at given target.
    void lookAt(const QVector3D& target);

    void scale(const QVector3D& scale);
    void scale(float scale);

    ChildSceneNodes::size_type numChildren() const;

    // precondition: index < numChildren
    // postcondition: valid child returned
    SceneNode* getChild(ChildSceneNodes::size_type index) const;

    // Detached child from the current node
    // postcondition: the child is not deleted
    virtual SceneNode* removeChild(ChildSceneNodes::size_type index);
    virtual SceneNode* removeChild(SceneNode* child);
    virtual void removeAllChildren();

    // Creates a new child and attaches it to the node
    // postcondition: valid child returned and attached to the node
    virtual SceneNode* createChild();

    // Attached a child node to the node
    // precondition: child != nullptr
    virtual void addChild(SceneNode* child);

    // Tells if the node's entities should cast shadows
    // Default light mask is 1 (LIGHT_CAST_SHADOWS)
    // SceneNodes are marked as occluders if the light's and node AND'ed is 1
    unsigned int lightMask() const;
    void setLightMask(unsigned int mask);

protected:
    void setParent(SceneNode* parent);

private:
    SceneNode* parent_;
    ChildSceneNodes children_;
    unsigned int lightMask_;

    bool localDirty_;
    bool childDirty_;

    QMatrix4x4 world_;
    QMatrix4x4 local_;

    void updateTransformation(bool updateWorld);

    // Sets the node's local bit dirty, and sets all parent's child bit dirty
    void markDirty();
};

}}

#endif //NODE_H