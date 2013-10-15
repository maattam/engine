#include "resourceloader.h"

namespace Engine {

template<typename Resource, typename... Args>
std::shared_ptr<Resource> ResourceDespatcher::get(const QString& fileName, Args&&... args)
{
    std::shared_ptr<Resource> resource;

    auto result = resources_.find(fileName);
    if(result == resources_.end())
    {
        resource = createResource<Resource>(fileName, args...);

        if(resource != nullptr)
        {
            resources_.insert(fileName, resource);
            watchResource(resource);
        }
    }

    else if(result->expired())
    {
        resource = createResource<Resource>(fileName, args...);

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

template<typename Resource, typename... Args>
std::shared_ptr<Resource> ResourceDespatcher::createResource(const QString& fn, Args&&... args)
{
    std::shared_ptr<Resource> resource = std::make_shared<Resource>(fn, args...);
    resource->despatcher_ = this;

    loader_->pushResource(resource);
    return resource;
}

}