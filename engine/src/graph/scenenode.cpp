#include "scenenode.h"

#include "sceneleaf.h"

#include <algorithm>

using namespace Engine;
using namespace Engine::Graph;

SceneNode::SceneNode()
    : Node(), castShadows_(true)
{
}

SceneNode::~SceneNode()
{
    detachAllEntities();
}

void SceneNode::attachEntity(SceneLeaf* entity)
{
    if(entity != nullptr)
    {
        entities_.push_back(entity);
        entity->attach(this);
    }
}

SceneLeaf* SceneNode::detachEntity(SceneLeaf* entity)
{
    if(entity == nullptr)
        return nullptr;

    auto iter = std::find(entities_.begin(), entities_.end(), entity);

    if(iter == entities_.end())
        return nullptr;

    SceneLeaf* child = *iter;
    child->detach();

    entities_.erase(iter);

    return child;
}

SceneLeaf* SceneNode::detachEntity(Entities::size_type index)
{
    SceneLeaf* child = getEntity(index);
    return detachEntity(child);
}

void SceneNode::detachAllEntities()
{
    for(SceneLeaf* ent : entities_)
    {
        ent->detach();
    }

    entities_.clear();
}

SceneNode::Entities::size_type SceneNode::numEntities() const
{
    return entities_.size();
}

SceneLeaf* SceneNode::getEntity(Entities::size_type index)
{
    if(index >= entities_.size())
        return nullptr;

    return entities_[index];
}

Node* SceneNode::createChildImpl()
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

SceneNode* SceneNode::createSceneNodeChild()
{
    return static_cast<SceneNode*>(createChild());
}

QList<SceneLeaf*> SceneNode::findEntities(const QString& name) const
{
    QList<SceneLeaf*> matches;

    // Find matches from this node
    for(SceneLeaf* entity : entities_)
    {
        if(entity->name() == name)
        {
            matches << entity;
        }
    }

    // Follow child nodes
    for(ChildNodes::size_type i = 0; i < numChildren(); ++i)
    {
        matches += static_cast<SceneNode*>(getChild(i))->findEntities(name);
    }

    return matches;
}