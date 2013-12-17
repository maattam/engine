#include "entity.h"

#include "graph/scenenode.h"

using namespace Engine::Graph;
using namespace Engine::Entity;

Entity::Entity()
    : node_(nullptr)
{
}

Entity::~Entity()
{
}

void Entity::updateAABB(const AABB& aabb)
{
    aabb_ = aabb;
}

const AABB& Entity::boundingBox() const
{
    return aabb_;
}

void Entity::attach(SceneNode* node)
{
    node_ = node;
}

void Entity::detach()
{
    node_ = nullptr;
}

SceneNode* Entity::parentNode() const
{
    return node_;
}

void Entity::setName(const QString& name)
{
    name_ = name;
}

const QString& Entity::name() const
{
    return name_;
}