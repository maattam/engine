#ifndef RESOURCEBASE_H
#define RESOURCEBASE_H

#include <QObject>
#include <QString>
#include <QStringList>

#include <atomic>

namespace Engine {

class ResourceDespatcher;
class ResourceData;
class ResourceLoader;

class ResourceBase : public QObject
{
Q_OBJECT

public:
    enum InitialisePolicy { ON_DEMAND, QUEUED };

    ResourceBase();
    ResourceBase(const QString& name, InitialisePolicy policy);
    virtual ~ResourceBase();

    // Synchronous data loading, returns false if the object is managed
    virtual bool load(const QString& fileName);

    // Initialises the resource from data.
    // Fails if the resource is managed.
    template<typename ResourceDataType>
    bool fromData(const ResourceDataType& data);

    // Attemps to initialise the resource from data, or return true if
    // the resource has already been initialised. Returns always true if the resource is not
    // managed.
    bool ready();

    // Returns the despatcher used in this context
    // The despatcher is null if the resource is not managed
    ResourceDespatcher* despatcher();

    // Called by ResourceDespatcher when the resource is managed.
    void setDespatcher(ResourceDespatcher* despatcher);

    // Deletes the loaded data
    // postcondition: data released if not already been released
    void release();

    // Tells if the resource is managed (created through ResourceDespatcher)
    bool managed() const;

    // Returns the resource's name (filename when the resource is managed)
    const QString& name() const;

    // InitialisePolicy tells when the resource should be loaded if the resource is managed.
    // ON_DEMAND means lazy loading, and QUEUED immediate loading when the resource's data is ready.
    InitialisePolicy initialisePolicy() const;

    // Reimplement to provide additional triggers for file watching
    virtual void queryFilesDebug(QStringList& files) const;

signals:
    // Called before the resource's data is released
    // precondition: data is present
    // postcondition: data has been deleted
    void released(const QString& name);

    // Called after the resource has been initialised successfully
    // precondition: ready is true
    // postcondition: data is valid
    void initialized(const QString& name);

protected:
    // Called upon resource initialisation
    // precondition: old data has been deleted
    // postcondition: new data has been returned, data != nullptr
    virtual ResourceData* createData() = 0;

    // Initialises the resource from data
    // precondition: data != nullptr
    // postcondition: true if initialisation was successful
    virtual bool initialise(ResourceData* data) = 0;

    // Called when data needs to be released and memory freed
    // precondition: data present
    // postcondition: data released
    virtual void releaseData() = 0;

private:
    friend class ResourceLoader;

    // Constructs a new ResourceData and deletes the old one
    ResourceData* createNewData();

    ResourceData* data_;
    ResourceDespatcher* despatcher_;

    std::atomic<bool> dataReady_;
    bool initialized_;
    bool released_;     // To prevent reloading
    QString name_;
    InitialisePolicy policy_;

    ResourceBase(const ResourceBase&);
    ResourceBase& operator=(const ResourceBase&);
};

}

#endif // RESOURCEBASE_H