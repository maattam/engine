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
#include <cassert>
#include <QThread>

class QFileSystemWatcher;

namespace Engine {

class ResourceBase;
class ResourceLoader;

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
    template<typename Resource, typename... Args>
    std::shared_ptr<Resource> get(const QString& fileName, Args&&... args);

signals:
    // Invokes the ResourceLoaders consume procedure
    void loadResources();

public slots:
    void fileChanged(const QString& path);

    // Called when the resource needs to be initialised
    // postcondition: if exists; attempted to initialise
    void resourceLoaded(const QString& id);

private:
    ResourceDespatcher(const ResourceDespatcher&);
    ResourceDespatcher& operator=(const ResourceDespatcher&);

    QHash<QString, std::weak_ptr<ResourceBase>> resources_;
    QThread loadThread_;
    ResourceLoader* loader_;

    QFileSystemWatcher* watcher_;

    // Allocates a new resource and loads it asynchronously
    template<typename Resource, typename... Args>
    std::shared_ptr<Resource> createResource(const QString& fn, Args&&... args);

    void watchResource(const std::shared_ptr<ResourceBase>& resource);
};

}

#include "resourcedespatcher.inl"

#endif // RESOURCEDESPATCHER_H