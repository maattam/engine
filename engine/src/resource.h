// Resource provides encapsulation for data that is loaded asynchronously and initialised on demand.

#ifndef RESOURCE_H
#define RESOURCE_H

#include <QString>
#include <QStringList>

#include <atomic>

namespace Engine {

class ResourceDespatcher;
class ResourceLoader;

class Resource
{
public:
    Resource();
    Resource(const QString& name);
    virtual ~Resource();

    // Synchronous data loading, returns false if the object is managed
    virtual bool load(const QString& fileName);

    // Tells whether the resource is managed by a ResourceDespatcher
    bool managed() const;

    // Tells whether the data for the resource has been loaded and initialized
    bool ready();

    const QString& name() const;

    void release();

protected:
    ResourceDespatcher* despatcher();

    // Called when the data needs to be loaded
    virtual bool loadData(const QString& fileName) = 0;

    // Called when loadData has returned successfully and the data has not yet been
    // initialized
    virtual bool initializeData() = 0;

    // Called when data needs to be released and memory freed
    virtual void releaseData() = 0;

    // Reimplement to provide additional triggers for file watching
    virtual void queryFilesDebug(QStringList& files) const;

private:
    friend class ResourceDespatcher;
    friend class ResourceLoader;

    ResourceDespatcher* despatcher_;
    std::atomic<bool> dataReady_;
    bool initialized_;
    QString name_;

    Resource(const Resource&);
    Resource& operator=(const Resource&);
};

}

#endif // RESOURCE_H