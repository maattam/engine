// Resource provides encapsulation for data that is loaded asynchronously and initialised on demand.

#ifndef RESOURCE_H
#define RESOURCE_H

#include <QString>
#include <QStringList>

#include <atomic>
#include <memory>
#include <cassert>

namespace Engine {

class ResourceDespatcher;
class ResourceLoader;

class ResourceData
{
public:
    ResourceData(ResourceDespatcher* despatcher);
    virtual ~ResourceData() {};

    virtual bool load(const QString& fileName) = 0;
    ResourceDespatcher* despatcher();

private:
    ResourceDespatcher* despatcher_;

    ResourceData(const ResourceData&);
    ResourceData& operator=(const ResourceData&);
};

class ResourceBase : public QObject
{
Q_OBJECT

public:
    ResourceBase();
    ResourceBase(const QString& name);
    virtual ~ResourceBase();

    // Synchronous data loading, returns false if the object is managed
    virtual bool load(const QString& fileName);

    bool ready();
    ResourceDespatcher* despatcher();
    void release();

    bool managed() const;
    const QString& name() const;

signals:
    void released();
    void initialized();

protected:
    virtual ResourceData* createData() = 0;
    virtual bool initialise(ResourceData* data) = 0;

    // Called when data needs to be released and memory freed
    virtual void releaseData() = 0;

    // Reimplement to provide additional triggers for file watching
    virtual void queryFilesDebug(QStringList& files) const;

private:
    friend class ResourceDespatcher;
    friend class ResourceLoader;

    // Constructs a new ResourceData and deletes the old one
    ResourceData* createNewData();

    ResourceData* data_;
    ResourceDespatcher* despatcher_;

    std::atomic<bool> dataReady_;
    bool initialized_;
    bool released_;     // To prevent reloading
    QString name_;

    ResourceBase(const ResourceBase&);
    ResourceBase& operator=(const ResourceBase&);
};

template<typename Type, typename ResourceDataType>
class Resource : public ResourceBase
{
public:
    Resource();
    Resource(const QString& name);

    typedef ResourceDataType DataType;
    typedef std::shared_ptr<Type> Ptr;

protected:
    virtual ResourceData* createData();
    virtual bool initialise(ResourceData* data);

    // Called when loadData has returned successfully and the data has not yet been
    // initialized
    virtual bool initialiseData(DataType& data) = 0;
};

#include "resource.inl"

}

#endif // RESOURCE_H