template<typename Type, typename ResourceDataType>
Resource<Type, ResourceDataType>::Resource() : ResourceBase()
{
}

template<typename Type, typename ResourceDataType>
Resource<Type, ResourceDataType>::Resource(const QString& name, InitialisePolicy policy)
    : ResourceBase(name, policy)
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

template<typename ResourceDataType>
bool ResourceBase::fromData(const ResourceDataType& data)
{
    if(managed())
        return false;

    if(initialized_)
    {
        release();
    }

    initialized_ = initialise(&data);
    if(initialized_)
    {
        releaseData();
    }

    return initialized_;
}