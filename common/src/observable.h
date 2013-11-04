#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <set>
#include <functional>

template<class TObserver>
class Observable
{
public:
    typedef TObserver ObserverType;

    Observable();
    virtual ~Observable();

    bool addObserver(ObserverType* obs);
    bool removeObserver(ObserverType* obs);

protected:
    // Notifies all observers
    template<typename... Args>
    void notify(void (ObserverType::*func)(Args...), Args... args) const;

private:
    std::set<ObserverType*> observers_;
};

#include "observable.inl"

#endif // OBSERVABLE_H