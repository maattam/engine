// Resource provides encapsulation for data that is loaded asynchronously and initialised on demand.

#ifndef RESOURCE_H
#define RESOURCE_H

#include <memory>

#include "resourcebase.h"

namespace Engine {

class ResourceLoader;

template<typename Type, typename ResourceDataType>
class Resource : public ResourceBase
{
public:
    Resource();
    Resource(const QString& name, InitialisePolicy policy = QUEUED);

    typedef ResourceDataType DataType;
    typedef std::shared_ptr<Type> Ptr;

    // Called upon resource initialisation
    // postcondition: new data has been returned, data != nullptr
    virtual ResourceDataPtr createData();

protected:
    // Initialises the resource from data
    // precondition: data != nullptr
    // postcondition: true if initialisation was successful
    virtual bool initialise(ResourceData* data);

    // Called when loadData has returned successfully and the data has not yet been
    // initialized
    virtual bool initialiseData(const DataType& data) = 0;
};

#include "resource.inl"

}

#endif // RESOURCE_H