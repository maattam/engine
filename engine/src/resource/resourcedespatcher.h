// ResourceDespatcher provides loose resource management through stl shared_ptr.
// The despatcher doesn't own the object handle, but shares the record to prevent
// allocating multiple instances of the same data.
// The despatcher also watches for filesystem changes to allow dynamic resource reloading
// during developement.

#ifndef RESOURCEDESPATCHER_H
#define RESOURCEDESPATCHER_H

#include <memory>

#include <QHash>
#include <QString>
#include <QDebug>
#include <cassert>
#include <QThread>

#include "resourceloader.h"

class QFileSystemWatcher;

namespace Engine {

class Resource;

class ResourceDespatcher : public QObject
{
    Q_OBJECT

public:
    ResourceDespatcher(QObject* parent = nullptr);
    ~ResourceDespatcher();

    // Clears the record; doesn't delete managed objects
    void clear();

    // Counts managed objects; expensive
    int numManaged() const;

    // Attempts to load a resource from storage asynchronously, or returns
    // a cached reference.
    // Note that the returned resource isn't guaranteed to be ready to use.
    template<typename T>
    std::shared_ptr<T> get(const QString& fileName);

signals:
    // Invokes the ResourceLoaders consume procedure
    void loadResources();

public slots:
    void fileChanged(const QString& path);

private:
    ResourceDespatcher(const ResourceDespatcher&);
    ResourceDespatcher& operator=(const ResourceDespatcher&);

    QHash<QString, std::weak_ptr<Resource>> resources_;
    QThread loadThread_;
    ResourceLoader* loader_;

    QFileSystemWatcher* watcher_;

    // Allocates a new resource and loads it asynchronously
    template<typename T>
    std::shared_ptr<T> createResource(const QString& fn);

    void watchResource(const std::shared_ptr<Resource>& resource);
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
        {
            resources_.insert(fileName, resource);
            watchResource(resource);
        }
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
        
        qDebug() << __FUNCTION__ << "Cached:" << fileName;
    }

    return resource;
}

template<typename T>
std::shared_ptr<T> ResourceDespatcher::createResource(const QString& fn)
{
    std::shared_ptr<T> resource = std::make_shared<T>(fn);
    resource->despatcher_ = this;

    loader_->pushResource(resource);
    return resource;
}

}

#endif // RESOURCEDESPATCHER_H