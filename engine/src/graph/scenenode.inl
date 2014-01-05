template<typename EntityType>
EntityType* SceneNode::findEntity(const QString& name) const
{
    for(Graph::SceneLeaf* entity : findEntities(name))
    {
        EntityType* ptr = dynamic_cast<EntityType*>(entity);
        if(ptr != nullptr)
        {
            return ptr;
        }
    }

    return nullptr;
}