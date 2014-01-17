//
//  Author   : Matti Määttä
//  Summary  : 
//

#ifndef OBSERVER_H
#define OBSERVER_H

#include "observable.h"

template<typename ObserverType>
class Observer
{
public:
    Observer();
    virtual ~Observer();

    typedef Observable<ObserverType> ObservableType;
    void _setObservable(ObservableType* obs);

private:
    ObservableType* observable_;
};

#include "observer.inl"

#endif // OBSERVER_H