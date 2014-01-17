//
//  Author   : Matti Määttä
//  Summary  : 
//

#include <utility>

template<class TObserver>
Observable<TObserver>::Observable()
{
}

template<class TObserver>
Observable<TObserver>::~Observable()
{
    for(TObserver* obs : observers_)
    {
        obs->_setObservable(nullptr);
    }
}

template<class TObserver>
bool Observable<TObserver>::addObserver(ObserverType* obs)
{
    if(obs == nullptr)
    {
        return false;
    }

    obs->_setObservable(this);
    return observers_.insert(obs).second;
}

template<class TObserver>
bool Observable<TObserver>::removeObserver(ObserverType* obs)
{
    if(observers_.erase(obs) == 1)
    {
        obs->_setObservable(nullptr);
        return true;
    }

    return false;
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