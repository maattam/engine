#include "entity.h"

using namespace Engine;

Entity::Entity()
    : castShadows_(false)
{
}

Entity::~Entity()
{
}

void Entity::setShadowCaster(bool shadows)
{
    castShadows_ = shadows;
}

bool Entity::isShadowCaster() const
{
    return castShadows_;
}