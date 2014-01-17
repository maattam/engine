//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "binder.h"

#include "bindable.h"

QMap<GLenum, GLuint> Binder::state_;

void Binder::reset()
{
    state_.clear();
}

bool Binder::test(Bindable& target, GLenum id)
{
    return testKey(id, target.handle());
}

bool Binder::test(Bindable& target)
{
    return testKey(target.type(), target.handle());
}

bool Binder::testKey(GLenum key, GLuint value)
{
    auto iter = state_.find(key);
    if(iter == state_.end())
    {
        state_[key] = value;
        return false;
    }

    if(iter.value() != value)
    {
        *iter = value;
        return false;
    }

    return true;
}