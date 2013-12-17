// ResourceDespatcher provides loose resource management through stl shared_ptr.
// The despatcher doesn't own the object handle, but shares the record to prevent
// allocating multiple instances of the same data.
// The despatcher also watches for filesystem changes to allow dynamic resource reloading
// during developement.

#ifndef WEAKRESOURCEDESPATCHER_H
#define WEAKRESOURCEDESPATCHER_H

#include "resourcedespatcher.h"

#include <QHash>
#include <cassert>
#include <QThread>

class QFileSystemWatcher;

namespace Engine {

class ResourceBase;
class ResourceLoader;

class WeakResourceDespatcher : public ResourceDespatcher
{
    Q_OBJECT

public:
    WeakResourceDespatcher(QObject* parent = nullptr);
    virtual ~WeakResourceDespatcher();

    // Clears the record; doesn't delete managed objects
    virtual void clear();

    // Counts managed objects; expensive
    virtual int numManaged() const;

signals:
    // Invokes the ResourceLoaders consume procedure
    void loadResources();

public slots:
    void fileChanged(const QString& path);

    // Called when the resource needs to be initialised
    // postcondition: if exists; attempted to initialise
    virtual void resourceLoaded(const QString& id);

protected:
    virtual WeakResourcePtr findResource(const QString& fileName);
    virtual void insertResource(const QString& fileName, const ResourcePtr& resource);

private:
    QHash<QString, WeakResourcePtr> resources_;
    QThread loadThread_;
    ResourceLoader* loader_;

    QFileSystemWatcher* watcher_;

    void watchResource(const ResourcePtr& resource);

    WeakResourceDespatcher(const WeakResourceDespatcher&);
    WeakResourceDespatcher& operator=(const WeakResourceDespatcher&);
};

}

#endif // WEAKRESOURCEDESPATCHER_H