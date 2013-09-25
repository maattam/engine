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
    // We don't want the data to be reloaded if the object is being managed by a despatcher
    if(managed())
    {
        qWarning() << __FUNCTION__ << "Attempted to invoke load on a managed resource.";
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

    // If the data has been read from the disk, we can initialise it
    else if(dataReady_)
    {
        initialized_ = initializeData();
        dataReady_ = false;

        if(initialized_)
            emit initialized();
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

void Resource::release()
{
    if(!name_.isEmpty())
    {
        qDebug() << __FUNCTION__ << "Releasing resource:" << name_;
    }

    initialized_ = false;
    dataReady_ = false;

    emit released();

    // Call implementation
    releaseData();
}

void Resource::queryFilesDebug(QStringList& files) const
{
    files.push_back(name_);
}