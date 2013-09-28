template<typename Type, typename ResourceDataType>
Resource<Type, ResourceDataType>::Resource() : ResourceBase()
{
}

template<typename Type, typename ResourceDataType>
Resource<Type, ResourceDataType>::Resource(const QString& name) : ResourceBase(name)
{
}

template<typename Type, typename ResourceDataType>
ResourceData* Resource<Type, ResourceDataType>::createData()
{
    return new ResourceDataType(despatcher());
}

template<typename Type, typename ResourceDataType>
bool Resource<Type, ResourceDataType>::initialise(ResourceData* data)
{
    ResourceDataType* rdata = dynamic_cast<ResourceDataType*>(data);
    assert(rdata != nullptr);

    return initialiseData(*rdata);
}