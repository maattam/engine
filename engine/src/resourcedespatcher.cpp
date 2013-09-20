#include "resourcedespatcher.h"

using namespace Engine;

ResourceDespatcher::ResourceDespatcher()
{
}

ResourceDespatcher::~ResourceDespatcher()
{
    clear();
}

void ResourceDespatcher::clear()
{
    resources_.clear();
}

int ResourceDespatcher::numManaged() const
{
    int count = 0;

    for(auto it = resources_.begin(); it != resources_.end(); ++it)
    {
        if(!it->expired())
            count++;
    }

    return count;
}