template<typename RenderableType>
std::weak_ptr<RenderableType> Primitive<RenderableType>::instance_;

template<typename RenderableType>
std::shared_ptr<RenderableType> Primitive<RenderableType>::instance()
{
    std::shared_ptr<RenderableType> shared = instance_.lock();
    if(!shared)
    {
        shared = std::make_shared<RenderableType>();
        instance_ = shared;
    }

    return shared;
}