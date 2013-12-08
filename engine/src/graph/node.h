// Abstract graph node which represents hierarchial rotation, scale and translation
// relative to child nodes.

#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QQuaternion>
#include <vector>

namespace Engine { namespace Graph {

class Node
{
public:
    typedef std::vector<Node*> ChildNodes;

    Node();
    virtual ~Node();

    // Returns nullptr the node is root of the hierarchy
    Node* getParent() const;

    // Caches the node's world transformation if the orientation, scale or
    // translation has changed.
    // If the node has children, the children's transformations are cached recursively.
    void update();

    // Returns the cached world transformation of this node.
    const QMatrix4x4& transformation() const;

    // Applies given transformation matrix to this node.
    void applyTransformation(const QMatrix4x4& matrix);

    // Set node position in world space
    void setPosition(const QVector3D& position);
    const QVector3D& position() const;
    const QVector3D& worldPosition() const;

    // Adds offset to the node's position
    void move(const QVector3D& offset);

    // Rotates the node's orientation by quaternion
    void rotate(const QQuaternion& quaternion);

    // Rotates the node's orientation by angle degrees along given axis
    void rotate(float angle, const QVector3D& axis);

    void setOrientation(const QQuaternion& quaternion);
    const QQuaternion& orientation() const;
    const QQuaternion& worldOrientation() const;

    // Makes the node face the given direction.
    void setDirection(const QVector3D& direction);

    // Returns the direction the node is facing
    QVector3D direction() const;

    // Helper function that sets the node to face at given target.
    void lookAt(const QVector3D& target);

    void setScale(const QVector3D& scale);
    void setScale(float scale);
    const QVector3D& scale() const;

    ChildNodes::size_type numChildren() const;

    // precondition: index < numChildren
    // postcondition: valid child returned
    Node* getChild(ChildNodes::size_type index);

    // Detached child from the current node
    // postcondition: the child is not deleted
    virtual Node* removeChild(ChildNodes::size_type index);
    virtual Node* removeChild(Node* child);
    virtual void removeAllChildren();

    // Creates a new child and attaches it to the node
    // postcondition: valid child returned and attached to the node
    virtual Node* createChild();

    // Attached a child node to the node
    // precondition: child != nullptr
    virtual void addChild(Node* child);

protected:
    void setParent(Node* parent);

    virtual Node* createChildImpl() = 0;

private:
    Node* parent_;
    ChildNodes children_;

    bool updateNeeded_;
    QMatrix4x4 cachedTransformation_;
    QMatrix4x4 cachedLocalTrans_;

    struct Transformation
    {
        QVector3D position;
        QVector3D scale;
        QQuaternion orientation;
    };

    Transformation localTrans_;
    Transformation cachedWorldTrans_;

    void updateTransformation(bool dirtyParent);
};

}}

#endif //NODE_H