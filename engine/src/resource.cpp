#include "resource.h"

#include <cassert>

#include <QDebug>

using namespace Engine;

Resource::Resource()
    : despatcher_(nullptr), dataReady_(false), initialized_(false)
{
}

Resource::Resource(const QString& name)
    : despatcher_(nullptr), dataReady_(false), initialized_(false), name_(name)
{
}

Resource::~Resource()
{
}

bool Resource::load(const QString& fileName)
{
    if(managed())
    {
        qDebug() << "Resource::load(): Constraint: Invoked load on a managed resource.";
        return false;
    }

    initialized_ = loadData(fileName) && initializeData();
    return initialized_;
}

bool Resource::ready()
{
    if(initialized_)
    {
        return true;
    }

    else if(dataReady_)
    {
        initialized_ = initializeData();
    }

    return initialized_;
}

bool Resource::managed() const
{
    return despatcher_ != nullptr;
}

ResourceDespatcher* Resource::despatcher()
{
    assert(despatcher_);
    return despatcher_;
}

const QString& Resource::name() const
{
    return name_;
}