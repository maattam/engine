//
//  Author   : Matti Määttä
//  Summary  : Abstract observer pattern.
//

#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <set>

template<class TObserver>
class Observable
{
public:
    typedef TObserver ObserverType;

    Observable();
    virtual ~Observable();

    virtual bool addObserver(ObserverType* obs);
    virtual bool removeObserver(ObserverType* obs);

protected:
    // Notifies all observers
    template<typename... Args>
    void notify(void (ObserverType::*func)(Args...), Args... args) const;

    // Notifies all observers
    // Returns false if any of the observers returned false.
    template<typename... Args>
    bool notify(bool (ObserverType::*func)(Args...), Args... args) const;

private:
    std::set<ObserverType*> observers_;

    Observable(const Observable&);
    Observable& operator=(const Observable&);
};

#include "observable.inl"

#endif // OBSERVABLE_H