#ifndef RESOURCE_H
#define RESOURCE_H

#include <QString>

namespace Engine {

class ResourceDespatcher;

class Resource
{
public:
    Resource();
    virtual ~Resource();

    virtual bool load(const QString& fileName) = 0;
    bool managed();

protected:
    ResourceDespatcher* despatcher();

private:
    friend class ResourceDespatcher;
    ResourceDespatcher* despatcher_;

    Resource(const Resource&);
    Resource& operator=(const Resource&);
};

}

#endif // RESOURCE_H