#include "resource.h"

#include <cassert>

using namespace Engine;

Resource::Resource() : despatcher_(nullptr)
{
}

Resource::~Resource()
{
}

bool Resource::managed()
{
    return despatcher_ != nullptr;
}

ResourceDespatcher* Resource::despatcher()
{
    assert(despatcher_);
    return despatcher_;
}