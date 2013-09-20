#ifndef RESOURCEDESPATCHER_H
#define RESOURCEDESPATCHER_H

#include <memory>

#include <QHash>
#include <QString>
#include <QDebug>
#include <cassert>

namespace Engine {

class Resource;

class ResourceDespatcher
{
public:
    ResourceDespatcher();
    ~ResourceDespatcher();

    void clear();

    // Counts managed objects; expensive
    int numManaged() const;

    template<typename T>
    std::shared_ptr<T> get(const QString& fileName);

private:
    ResourceDespatcher(const ResourceDespatcher&);
    ResourceDespatcher& operator=(const ResourceDespatcher&);

    QHash<QString, std::weak_ptr<Resource>> resources_;

    template<typename T>
    std::shared_ptr<T> createResource(const QString& fn);
};

// Template implementations
template<typename T>
std::shared_ptr<T> ResourceDespatcher::get(const QString& fileName)
{
    std::shared_ptr<T> resource;

    auto result = resources_.find(fileName);
    if(result == resources_.end())
    {
        resource = createResource<T>(fileName);

        if(resource != nullptr)
            resources_.insert(fileName, resource);
    }

    else if(result->expired())
    {
        resource = createResource<T>(fileName);

        if(resource != nullptr)
            *result = resource;
    }

    else
    {
        resource = std::dynamic_pointer_cast<T>(result->lock());
        assert(resource);
        
        qDebug() << "Using cached resource: " << fileName;
    }

    return resource;
}

template<typename T>
std::shared_ptr<T> ResourceDespatcher::createResource(const QString& fn)
{
    std::shared_ptr<T> resource = std::make_shared<T>();
    resource->despatcher_ = this;

    if(!resource->load(fn))
    {
        qDebug() << "Failed to load resource: " << fn;
        resource.reset();
    }

    else
    {
        qDebug() << "Loaded resource: " << fn;
    }

    return resource;
}

}

#endif // RESOURCEDESPATCHER_H