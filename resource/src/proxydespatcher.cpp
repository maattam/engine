//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "proxydespatcher.h"

#include "resourcebase.h"

using namespace Engine;

ProxyDespatcher::ProxyDespatcher(QObject* parent)
    : ResourceDespatcher(parent), target_(nullptr)
{
}

ProxyDespatcher::~ProxyDespatcher()
{
}

void ProxyDespatcher::clear()
{
    resources_.clear();
}

int ProxyDespatcher::numManaged() const
{
    int count = 0;

    for(auto it = resources_.begin(); it != resources_.end(); ++it)
    {
        if(!it->expired())
            count++;
    }

    return count;
}

void ProxyDespatcher::setTarget(ResourceDespatcher* despatcher)
{
    target_ = despatcher;
    connect(this, &ProxyDespatcher::moveResource, despatcher, &ResourceDespatcher::loadResource);
}

void ProxyDespatcher::loadResource(ResourcePtr resource)
{
    insertResource(resource->name(), resource);
}

ProxyDespatcher::WeakResourcePtr ProxyDespatcher::findResource(const QString& fileName)
{
    WeakResourcePtr weakPtr;

    auto result = resources_.find(fileName);
    if(result != resources_.end())
    {
        weakPtr = result.value();
    }

    return weakPtr;
}

void ProxyDespatcher::insertResource(const QString& fileName, const ResourcePtr& resource)
{
    resources_.insert(fileName, resource);
    resource->setDespatcher(this);

    // Call target despatcher
    emit moveResource(resource);
}