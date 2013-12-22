template<typename EntityType>
EntityType* SceneNode::findEntity(const QString& name) const
{
    for(Entity::Entity* entity : findEntities(name))
    {
        EntityType* ptr = dynamic_cast<EntityType*>(entity);
        if(ptr != nullptr)
        {
            return ptr;
        }
    }

    return nullptr;
}