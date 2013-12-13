#include <utility>

template<class TObserver>
Observable<TObserver>::Observable()
{
}

template<class TObserver>
Observable<TObserver>::~Observable()
{
    observers_.clear();
}

template<class TObserver>
bool Observable<TObserver>::addObserver(ObserverType* obs)
{
    if(obs == nullptr)
    {
        return false;
    }

    return observers_.insert(obs).second;
}

template<class TObserver>
bool Observable<TObserver>::removeObserver(ObserverType* obs)
{
    return observers_.erase(obs) == 1;
}

template<class TObserver>
template<typename... Args>
void Observable<TObserver>::notify(void (ObserverType::*func)(Args...), Args... args) const
{
    for(ObserverType* obs : observers_)
    {
        (obs->*func)(std::forward<Args>(args)...);
    }
}

template<class TObserver>
template<typename... Args>
bool Observable<TObserver>::notify(bool (ObserverType::*func)(Args...), Args... args) const
{
    bool ret = true;

    for(ObserverType* obs : observers_)
    {
        if(!(obs->*func)(std::forward<Args>(args)...))
        {
            ret = false;
        }
    }

    return ret;
}