template<class ObserverType>
Observer<ObserverType>::Observer()
    : observable_(nullptr)
{
}

template<class ObserverType>
Observer<ObserverType>::~Observer()
{
    if(observable_ != nullptr)
    {
        observable_->removeObserver(static_cast<ObserverType*>(this));
    }
}

template<class ObserverType>
void Observer<ObserverType>::_setObservable(ObservableType* obs)
{
    observable_ = obs;
}