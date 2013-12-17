template<typename Resource, typename... Args>
std::shared_ptr<Resource> ResourceDespatcher::get(const QString& fileName, Args&&... args)
{
    std::shared_ptr<Resource> result;

    ResourcePtr resource = findResource(fileName).lock();
    if(resource == nullptr)
    {
        // Resource doesn't exist or has been deleted.
        result = std::make_shared<Resource>(fileName, args...);
        insertResource(fileName, result);
    }

    else
    {
        result = std::dynamic_pointer_cast<Resource>(resource);
    }

    return result;
}