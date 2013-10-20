#include "node.h"

#include "mathelp.h"

#include <algorithm>
#include <qDebug>

using namespace Engine;
using namespace Engine::Graph;

Node::Node()
    : parent_(nullptr)
{
    scale_ = QVector3D(1.0f, 1.0f, 1.0f);
    updateNeeded_ = true;
}

Node::~Node()
{
    for(Node* child : children_)
        delete child;
}

Node* Node::getParent() const
{
    return parent_;
}

void Node::update()
{
    // Update children
    updateTransformation(QMatrix4x4(), updateNeeded_);
}

const QMatrix4x4& Node::transformation() const
{
    return cachedTransformation_;
}

void Node::updateTransformation(const QMatrix4x4& parentTransformation, bool dirtyParent)
{
    bool dirty = updateNeeded_ | dirtyParent;

    // Update local transformation if the node needs updating
    if(updateNeeded_)
    {
        cachedLocalTrans_.setToIdentity();
        cachedLocalTrans_.translate(position_);
        cachedLocalTrans_.rotate(orientation_);
        cachedLocalTrans_.scale(scale_);

        updateNeeded_ = false;
    }

    // If parent or local transformation has changed; we can't rely on old cachedTransformation
    if(dirty)
    {
        cachedTransformation_ = parentTransformation * cachedLocalTrans_;
    }

    // Walk through child nodes
    for(Node* child : children_)
    {
        child->updateTransformation(cachedTransformation_, dirty);
    }
}

void Node::applyTransformation(const QMatrix4x4& matrix)
{
    const QVector3D right = matrix.column(0).toVector3D();
    const QVector3D up = matrix.column(1).toVector3D();
    const QVector3D forward = matrix.column(2).toVector3D();
    
    // Translation is the 4th column
    move(matrix.column(3).toVector3D());

    // Scale is the vector norm
    scale_ *= QVector3D(right.length(), up.length(), forward.length());

    // Calculate orientation from axes
    rotate(orientationFromAxes(right.normalized(), up.normalized(), forward.normalized()));
}

Node::ChildNodes::size_type Node::numChildren() const
{
    return children_.size();
}

Node* Node::getChild(ChildNodes::size_type index)
{
    if(index >= children_.size())
        return nullptr;

    return children_[index];
}

void Node::addChild(Node* child)
{
    if(child != nullptr)
        children_.push_back(child);
}

Node* Node::removeChild(ChildNodes::size_type index)
{
    Node* child = getChild(index);

    if(child != nullptr)
    {
        children_.erase(children_.begin() + index);
    }

    return child;
}

Node* Node::removeChild(Node* child)
{
    auto iter = std::find(children_.begin(), children_.end(), child);

    if(iter == children_.end())
        return nullptr;

    Node* found = *iter;
    children_.erase(iter);

    return found;
}

void Node::removeAllChildren()
{
    children_.clear();
}

Node* Node::createChild()
{
    Node* child = createChildImpl();
    children_.push_back(child);

    child->setParent(this);

    return child;
}

void Node::setParent(Node* parent)
{
    parent_ = parent;
}

void Node::setPosition(const QVector3D& position)
{
    position_ = position;
    updateNeeded_ = true;
}

const QVector3D& Node::position() const
{
    return position_;
}

void Node::move(const QVector3D& offset)
{
    position_ += offset;
    updateNeeded_ = true;
}

void Node::rotate(const QQuaternion& quaternion)
{
    setOrientation(quaternion.normalized() * orientation_);
}

void Node::rotate(float angle, const QVector3D& axis)
{
    rotate(QQuaternion::fromAxisAndAngle(axis, angle));
}

void Node::setOrientation(const QQuaternion& quaternion)
{
    orientation_ = quaternion;
    updateNeeded_ = true;
}

const QQuaternion& Node::orientation() const
{
    return orientation_;
}

void Node::setDirection(const QVector3D& direction)
{
    // Default look in OpenGL is towards -Z
    const QVector3D start = -UNIT_Z;
    const QVector3D dest = direction.normalized();

    QQuaternion ori;

    // 180 degree turn; infite possibilites, choose one that works.
    if(QVector3D::dotProduct(start, dest) < -1.0f + 0.001f)
    {
        ori = QQuaternion(0.0f, 0.0f, 1.0f, 0.0f);
    }

    else
    {
        ori = rotationBetweenVectors(start, dest);
    }

    setOrientation(ori.normalized());
}

QVector3D Node::direction() const
{
    // Default direction points towards -Z
    return orientation_.rotatedVector(-UNIT_Z);
}

void Node::lookAt(const QVector3D& target)
{
    setDirection(target - position_);
}

void Node::setScale(const QVector3D& scale)
{
    scale_ = scale;
    updateNeeded_ = true;
}

void Node::setScale(float scale)
{
    scale_ = QVector3D(scale, scale, scale);
    updateNeeded_ = true;
}

const QVector3D& Node::scale() const
{
    return scale_;
}