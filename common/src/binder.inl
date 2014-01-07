template<typename BindableDerived, typename... Args>
bool Binder::bind(BindableDerived& target, Args&&... args)
{
    if(!test(target, std::forward<Args>(args)...))
    {
        return target.bind(std::forward<Args>(args)...);
    }

    return true; // Object is already bound
}

template<typename BindableDerived, typename... Args>
bool Binder::bind(const std::shared_ptr<BindableDerived>& target, Args&&... args)
{
    if(!target)
    {
        return false;
    }

    return bind(*target, args...);
}

template<typename BindableDerived, typename... Args>
bool Binder::bind(BindableDerived* target, Args&&... args)
{
    if(target == nullptr)
    {
        return false;
    }

    return bind(*target, args...);
}