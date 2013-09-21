#include "entity.h"

using namespace Engine::Entity;

Entity::Entity()
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