#ifndef RESOURCEDATA_H
#define RESOURCEDATA_H

#include <QStringList>

namespace Engine {

class ResourceDespatcher;

class ResourceData
{
public:
    ResourceData();
    virtual ~ResourceData() {};

    // Attemps to load the given file from disk
    // precondition: true returned if loading has been successful
    virtual bool load(const QString& fileName) = 0;

    // Returns the despatcher used in this context
    // If the resource is not managed, the despatcher can be null
    ResourceDespatcher* despatcher();

    void setDespatcher(ResourceDespatcher* despatcher);

    // Reimplement to provide additional triggers for file watching
    virtual QStringList queryFilesDebug() const;

private:
    ResourceDespatcher* despatcher_;

    ResourceData(const ResourceData&);
    ResourceData& operator=(const ResourceData&);
};

}

#endif // RESOURCEDATA_H