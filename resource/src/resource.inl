//
//  Author   : Matti Määttä
//  Summary  : 
//

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
ResourceBase::ResourceDataPtr Resource<Type, ResourceDataType>::createData()
{
    return std::make_shared<ResourceDataType>();
}

template<typename Type, typename ResourceDataType>
bool Resource<Type, ResourceDataType>::initialise(ResourceData* data)
{
    ResourceDataType* rdata = dynamic_cast<ResourceDataType*>(data);
    Q_ASSERT(rdata != nullptr);

    return initialiseData(*rdata);
}