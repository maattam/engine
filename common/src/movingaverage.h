// MovingAverage does average filtering on the inserted values:
// calcAverage := sum(v, 0, samples) / samples

#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <QVector>

template<
    typename ValueType,
    typename ReturnType,
    int samples,
    ValueType initial = ValueType()
>
class MovingAverage
{
public:
    MovingAverage()
        : samples_(samples, initial)
    {
    }

    void push(const ValueType& value)
    {
        samples_.pop_front();
        samples_.push_back(value);
    }

    ReturnType calcAverage() const
    {
        ReturnType sum = 0;
        for(int i = 0; i < samples; ++i)
        {
            sum += samples_[i];
        }

        return static_cast<ReturnType>(sum / samples);
    }

    MovingAverage& operator<<(const ValueType& value)
    {
        push(value);
        return *this;
    }

    operator ReturnType() const
    {
        return calcAverage();
    }

private:
    QVector<ValueType> samples_;
};

#endif // MOVINGAVERAGE_H