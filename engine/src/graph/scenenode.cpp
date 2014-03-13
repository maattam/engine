//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "scenenode.h"

#include "mathelp.h"
#include "light.h"

#include <algorithm>

using namespace Engine;
using namespace Engine::Graph;

SceneNode::SceneNode()
    : parent_(nullptr), lightMask_(Light::MASK_CAST_SHADOWS), localDirty_(true), childDirty_(false)
{
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

void SceneNode::setParent(SceneNode* parent)
{
    parent_ = parent;
    markDirty();
}

void SceneNode::propagate()
{
    // Update children
    updateTransformation(false);
}

const QMatrix4x4& SceneNode::transformation() const
{
    return world_;
}

void SceneNode::updateTransformation(bool updateWorld)
{
    // Update world transformation if node is dirty
    updateWorld = updateWorld | localDirty_;

    // If parent or local transformation has changed; we can't rely on old world transformation
    if(updateWorld)
    {
        SceneNode* parent = getParent();
        if(parent != nullptr)
        {
            world_ = parent->world_ * local_;
        }

        else
        {
            world_ = local_;
        }
    }

    // Walk through child nodes
    if(childDirty_ || updateWorld)
    {
        for(SceneNode* child : children_)
        {
            child->updateTransformation(updateWorld);
        }

        childDirty_ = false;
    }
}

void SceneNode::applyTransformation(const QMatrix4x4& matrix)
{
    local_ = matrix * local_;
    markDirty();
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
    childDirty_ = true;

    return child;
}

void SceneNode::setPosition(const QVector3D& position)
{
    local_.setColumn(3, QVector4D(position, 1.0f));
    markDirty();
}

QVector3D SceneNode::position() const
{
    return extractTranslation(local_);
}

void SceneNode::move(const QVector3D& offset)
{
    local_.translate(offset);
    markDirty();
}

void SceneNode::rotate(const QQuaternion& quaternion)
{
    local_.rotate(quaternion);
    markDirty();
}

void SceneNode::rotate(float angle, const QVector3D& axis)
{
    local_.rotate(angle, axis);
    markDirty();
}

void SceneNode::setOrientation(const QQuaternion& quaternion)
{
    // Save scale
    QVector3D scale = extractScale(local_);

    // Reset the top-left 3x3 matrix
    float* mat = local_.data();
    mat[0] = 1.0f; mat[1] = 0.0f; mat[2]  = 0.0f;
    mat[4] = 0.0f; mat[5] = 1.0f; mat[6]  = 0.0f;
    mat[8] = 0.0f; mat[9] = 0.0f; mat[10] = 1.0f;

    local_.optimize();

    // Re-do transformations
    local_.scale(scale);
    local_.rotate(quaternion);

    markDirty();
}

QQuaternion SceneNode::orientation() const
{
    return extractOrientation(local_);
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
    return local_.mapVector(-UNIT_Z).normalized();
}

void SceneNode::lookAt(const QVector3D& target)
{
    setDirection(target - worldPosition());
}

void SceneNode::scale(const QVector3D& scale)
{
    local_.scale(scale);
    markDirty();
}

void SceneNode::scale(float scale)
{
    local_.scale(scale);
    markDirty();
}

QVector3D SceneNode::worldPosition() const
{
    return extractTranslation(world_);
}

QQuaternion SceneNode::worldOrientation() const
{
    return extractOrientation(world_);
}

unsigned int SceneNode::lightMask() const
{
    return lightMask_;
}

void SceneNode::setLightMask(unsigned int mask)
{
    lightMask_ = mask;
}

void SceneNode::markDirty()
{
    localDirty_ = true;
    
    // Propage upwards
    SceneNode* parent = getParent();
    while(parent != nullptr && !parent->childDirty_)
    {
        parent->childDirty_ = true;
    }
}