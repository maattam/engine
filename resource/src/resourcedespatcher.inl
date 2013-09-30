template<typename Resource>
std::shared_ptr<Resource> ResourceDespatcher::get(const QString& fileName)
{
    std::shared_ptr<Resource> resource;

    auto result = resources_.find(fileName);
    if(result == resources_.end())
    {
        resource = createResource<Resource>(fileName);

        if(resource != nullptr)
        {
            resources_.insert(fileName, resource);
            watchResource(resource);
        }
    }

    else if(result->expired())
    {
        resource = createResource<Resource>(fileName);

        if(resource != nullptr)
            *result = resource;
    }

    else
    {
        resource = std::dynamic_pointer_cast<Resource>(result->lock());
        assert(resource);
    }

    return resource;
}

template<typename Resource>
std::shared_ptr<Resource> ResourceDespatcher::createResource(const QString& fn)
{
    std::shared_ptr<Resource> resource = std::make_shared<Resource>(fn);
    resource->despatcher_ = this;

    loader_->pushResource(resource);
    return resource;
}