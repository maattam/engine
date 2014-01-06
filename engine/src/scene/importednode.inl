    template<typename LeafType>
    std::shared_ptr<LeafType> ImportedNode::findLeaf(const QString& name)
    {
        for(const ImportedNodeData::EntityPtr& leaf : entities_)
        {
            if(leaf->name() == name)
            {
                return std::dynamic_pointer_cast<LeafType>(leaf);
            }
        }

        return nullptr;
    }