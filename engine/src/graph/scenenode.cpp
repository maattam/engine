#include "scenenode.h"

#include "entity/entity.h"

#include <algorithm>

using namespace Engine;
using namespace Engine::Graph;

SceneNode::SceneNode()
    : Node(), castShadows_(true)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::attachEntity(Entity::Entity* entity)
{
    if(entity != nullptr)
    {
        entities_.push_back(entity);
    }
}

Entity::Entity* SceneNode::detachEntity(Entity::Entity* entity)
{
    auto iter = std::find(entities_.begin(), entities_.end(), entity);

    if(iter == entities_.end())
        return nullptr;

    Entity::Entity* child = *iter;
    entities_.erase(iter);

    return child;
}

Entity::Entity* SceneNode::detachEntity(Entities::size_type index)
{
    Entity::Entity* child = getEntity(index);

    if(child != nullptr)
    {
        entities_.erase(entities_.begin() + index);
    }

    return child;
}

void SceneNode::detachAllEntities()
{
    entities_.clear();
}

SceneNode::Entities::size_type SceneNode::numEntities() const
{
    return entities_.size();
}

Entity::Entity* SceneNode::getEntity(Entities::size_type index)
{
    if(index >= entities_.size())
        return nullptr;

    return entities_[index];
}

Node* SceneNode::createChildImpl(const QMatrix4x4& transformation)
{
    return new SceneNode;
}

bool SceneNode::isShadowCaster() const
{
    return castShadows_;
}

void SceneNode::setShadowCaster(bool shadows)
{
    castShadows_ = shadows;
}