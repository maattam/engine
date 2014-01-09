// ResourceDespatcher provides loose resource management through stl shared_ptr.
// The despatcher doesn't own the object handle, but shares the record to prevent
// allocating multiple instances of the same data.
// The despatcher also watches for filesystem changes to allow dynamic resource reloading
// during developement.

#ifndef WEAKRESOURCEDESPATCHER_H
#define WEAKRESOURCEDESPATCHER_H

#include "resourcedespatcher.h"

#include <QHash>
#include <QThreadPool>

class QFileSystemWatcher;

namespace Engine {

class ResourceBase;
class ResourceData;

class WeakResourceDespatcher : public ResourceDespatcher
{
    Q_OBJECT

public:
    explicit WeakResourceDespatcher(unsigned int threadCount = 2, QObject* parent = nullptr);
    virtual ~WeakResourceDespatcher();

    // Clears the record; doesn't delete managed objects
    virtual void clear();

    // Counts managed objects; expensive
    virtual int numManaged() const;

    typedef std::shared_ptr<ResourceData> ResourceDataPtr;

public slots:
    void fileChanged(const QString& path);

    // Called when the resource needs to be initialised
    // postcondition: if exists; attempted to initialise
    void resourceLoaded(QString id, ResourceDataPtr data);

    // Can be used to move resources between despatchers.
    // The resource is loaded by the target despatcher and ownership is copied.
    // Precondition: Resource name must be the file name, otherwise loading will fail.
    virtual void loadResource(ResourcePtr resource);

protected:
    virtual WeakResourcePtr findResource(const QString& fileName);
    virtual void insertResource(const QString& fileName, const ResourcePtr& resource);

private:
    QHash<QString, WeakResourcePtr> resources_;
    QThreadPool threadPool_;
    QString rootDirectory_;

    QFileSystemWatcher* watcher_;

    void watchResource(const ResourcePtr& resource);
    void pushResource(const QString& fileName, const ResourcePtr& resource);

    WeakResourceDespatcher(const WeakResourceDespatcher&);
    WeakResourceDespatcher& operator=(const WeakResourceDespatcher&);
};

}

#endif // WEAKRESOURCEDESPATCHER_H