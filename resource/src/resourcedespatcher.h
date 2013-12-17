// Interface for different resource despatchers.

#ifndef RESOURCEDESPATCHER_H
#define RESOURCEDESPATCHER_H

#include <memory>

#include <QObject>
#include <QString>

namespace Engine {

class ResourceBase;
class ResourceData;

class ResourceDespatcher : public QObject
{
    Q_OBJECT

public:
    explicit ResourceDespatcher(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ResourceDespatcher() {};

    // Clears the record; doesn't delete managed objects
    virtual void clear() = 0;

    // Counts managed objects; expensive
    virtual int numManaged() const = 0;

    // Attempts to load a resource from storage asynchronously, or returns
    // a cached reference.
    // Note that the returned resource isn't guaranteed to be ready to use.
    template<typename ResourceType, typename... Args>
    std::shared_ptr<ResourceType> get(const QString& fileName, Args&&... args);

    typedef std::shared_ptr<ResourceBase> ResourcePtr;

public slots:
    // Can be used to move resources between despatchers.
    // The resource is loaded by the target despatcher and ownership is copied.
    virtual void loadResource(ResourcePtr resource) = 0;

protected:
    typedef std::weak_ptr<ResourceBase> WeakResourcePtr;

    virtual WeakResourcePtr findResource(const QString& fileName) = 0;
    virtual void insertResource(const QString& fileName, const ResourcePtr& resource) = 0;
};

#include "resourcedespatcher.inl"

}

#endif // RESOURCEDESPATCHER_H