#include "scenenode.h"

#include "mathelp.h"
#include "light.h"

#include <algorithm>
#include <qDebug>

using namespace Engine;
using namespace Engine::Graph;

SceneNode::SceneNode()
    : parent_(nullptr), lightMask_(Light::MASK_CAST_SHADOWS)
{
    localTrans_.scale = QVector3D(1.0f, 1.0f, 1.0f);
    updateNeeded_ = true;
}

SceneNode::~SceneNode()
{
    for(SceneNode* child : children_)
    {
        delete child;
    }
}

SceneNode* SceneNode::getParent() const
{
    return parent_;
}

void SceneNode::propagate()
{
    // Update children
    updateTransformation(updateNeeded_);
}

const QMatrix4x4& SceneNode::transformation() const
{
    return cachedTransformation_;
}

void SceneNode::updateTransformation(bool dirtyParent)
{
    bool dirty = updateNeeded_ | dirtyParent;

    // Update local transformation if the node needs updating
    if(updateNeeded_)
    {
        cachedLocalTrans_.setToIdentity();
        cachedLocalTrans_.translate(localTrans_.position);
        cachedLocalTrans_.rotate(localTrans_.orientation);
        cachedLocalTrans_.scale(localTrans_.scale);

        updateNeeded_ = false;
    }

    // If parent or local transformation has changed; we can't rely on old cachedTransformation
    if(dirty)
    {
        SceneNode* parent = getParent();
        if(parent != nullptr)
        {
            cachedTransformation_ = parent->transformation() * cachedLocalTrans_;
            
            const Transformation& trans = parent->cachedWorldTrans_;
            cachedWorldTrans_.position = trans.orientation.rotatedVector(localTrans_.position) + trans.position;
            cachedWorldTrans_.orientation = trans.orientation * localTrans_.orientation;
        }

        else
        {
            cachedTransformation_ = cachedLocalTrans_;
            cachedWorldTrans_ = localTrans_;
        }
    }

    // Walk through child nodes
    for(SceneNode* child : children_)
    {
        child->updateTransformation(dirty);
    }
}

void SceneNode::applyTransformation(const QMatrix4x4& matrix)
{
    const QVector3D right = matrix.column(0).toVector3D();
    const QVector3D up = matrix.column(1).toVector3D();
    const QVector3D forward = matrix.column(2).toVector3D();
    
    // Translation is the 4th column
    move(matrix.column(3).toVector3D());

    // Scale is the vector norm
    localTrans_.scale *= QVector3D(right.length(), up.length(), forward.length());

    // Calculate orientation from axes
    rotate(orientationFromAxes(right.normalized(), up.normalized(), forward.normalized()));
}

SceneNode::ChildSceneNodes::size_type SceneNode::numChildren() const
{
    return children_.size();
}

SceneNode* SceneNode::getChild(ChildSceneNodes::size_type index) const
{
    if(index >= children_.size())
        return nullptr;

    return children_[index];
}

void SceneNode::addChild(SceneNode* child)
{
    if(child != nullptr)
    {
        child->setParent(this);
        children_.push_back(child);
    }
}

SceneNode* SceneNode::removeChild(ChildSceneNodes::size_type index)
{
    SceneNode* child = getChild(index);

    if(child != nullptr)
    {
        children_.erase(children_.begin() + index);
    }

    return child;
}

SceneNode* SceneNode::removeChild(SceneNode* child)
{
    auto iter = std::find(children_.begin(), children_.end(), child);

    if(iter == children_.end())
        return nullptr;

    SceneNode* found = *iter;
    children_.erase(iter);

    return found;
}

void SceneNode::removeAllChildren()
{
    children_.clear();
}

SceneNode* SceneNode::createChild()
{
    SceneNode* child = new SceneNode();
    children_.push_back(child);

    child->setParent(this);

    return child;
}

void SceneNode::setParent(SceneNode* parent)
{
    parent_ = parent;
}

void SceneNode::setPosition(const QVector3D& position)
{
    localTrans_.position = position;
    updateNeeded_ = true;
}

const QVector3D& SceneNode::position() const
{
    return localTrans_.position;
}

void SceneNode::move(const QVector3D& offset)
{
    localTrans_.position += offset;
    updateNeeded_ = true;
}

void SceneNode::rotate(const QQuaternion& quaternion)
{
    setOrientation(quaternion.normalized() * localTrans_.orientation);
}

void SceneNode::rotate(float angle, const QVector3D& axis)
{
    rotate(QQuaternion::fromAxisAndAngle(axis, angle));
}

void SceneNode::setOrientation(const QQuaternion& quaternion)
{
    localTrans_.orientation = quaternion.normalized();
    updateNeeded_ = true;
}

const QQuaternion& SceneNode::orientation() const
{
    return localTrans_.orientation;
}

void SceneNode::setDirection(const QVector3D& direction)
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

QVector3D SceneNode::direction() const
{
    // Default direction points towards -Z
    return localTrans_.orientation.rotatedVector(-UNIT_Z);
}

void SceneNode::lookAt(const QVector3D& target)
{
    setDirection(target - cachedWorldTrans_.position);
}

void SceneNode::setScale(const QVector3D& scale)
{
    localTrans_.scale = scale;
    updateNeeded_ = true;
}

void SceneNode::setScale(float scale)
{
    setScale(QVector3D(scale, scale, scale));
}

const QVector3D& SceneNode::scale() const
{
    return localTrans_.scale;
}

const QVector3D& SceneNode::worldPosition() const
{
    return cachedWorldTrans_.position;
}

const QQuaternion& SceneNode::worldOrientation() const
{
    return cachedWorldTrans_.orientation;
}

unsigned int SceneNode::lightMask() const
{
    return lightMask_;
}

void SceneNode::setLightMask(unsigned int mask)
{
    lightMask_ = mask;
}