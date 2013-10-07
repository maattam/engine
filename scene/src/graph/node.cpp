#include "node.h"

#include <algorithm>

using namespace Engine;
using namespace Engine::Graph;

Node::Node()
    : parent_(nullptr)
{
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

const QMatrix4x4& Node::transformation() const
{
    return transformation_;
}

void Node::applyTransformation(const QMatrix4x4& matrix)
{
    transformation_ *= matrix;
}

void Node::setPosition(const QVector3D& position)
{
    // Translation is the 3rd column vector of our transformation matrix
    transformation_.setColumn(3, QVector4D(position, 1.0f));
}

const QVector3D& Node::position() const
{
    // Discard the W coordinate
    return transformation_.column(3).toVector3D();
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

Node* Node::createChild(const QMatrix4x4& transformation)
{
    Node* child = createChildImpl(transformation);
    children_.push_back(child);

    child->setParent(this);
    child->applyTransformation(transformation);

    return child;
}

void Node::setParent(Node* parent)
{
    parent_ = parent;
}