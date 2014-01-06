#include "sceneleaf.h"

#include "scenenode.h"

using namespace Engine;
using namespace Engine::Graph;

SceneLeaf::SceneLeaf()
    : node_(nullptr)
{
}

SceneLeaf::~SceneLeaf()
{
}

void SceneLeaf::updateAABB(const AABB& aabb)
{
    aabb_ = aabb;
}

const AABB& SceneLeaf::boundingBox() const
{
    return aabb_;
}

void SceneLeaf::attach(SceneNode* node)
{
    node_ = node;
}

void SceneLeaf::detach()
{
    node_ = nullptr;
}

Graph::SceneNode* SceneLeaf::parentNode() const
{
    return node_;
}

void SceneLeaf::setName(const QString& name)
{
    name_ = name;
}

const QString& SceneLeaf::name() const
{
    return name_;
}