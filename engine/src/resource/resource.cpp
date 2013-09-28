#include "resource.h"

#include <cassert>

#include <QDebug>

using namespace Engine;

ResourceData::ResourceData(ResourceDespatcher* despatcher)
    : despatcher_(despatcher)
{
}

ResourceDespatcher* ResourceData::despatcher()
{
    return despatcher_;
}

//
// ResourceBase
//

ResourceBase::ResourceBase()
    : data_(nullptr), despatcher_(nullptr), dataReady_(false), initialized_(false)
{
}

ResourceBase::ResourceBase(const QString& name)
    : data_(nullptr), despatcher_(nullptr), dataReady_(false), initialized_(false), name_(name)
{
}

ResourceBase::~ResourceBase()
{
    if(data_ != nullptr)
    {
        delete data_;
        data_ = nullptr;
    }
}

ResourceData* ResourceBase::createNewData()
{
    dataReady_ = false;

    if(data_ != nullptr)
        delete data_;

    data_ = createData();
    return data_;
}

bool ResourceBase::load(const QString& fileName)
{
    // We don't want the data to be reloaded if the object is being managed by a despatcher
    if(managed())
    {
        qWarning() << __FUNCTION__ << "Attempted to invoke load on a managed resource.";
        return false;
    }

    data_ = createNewData();
    if(!data_->load(fileName))
    {
        initialized_ = false;
    }

    else
    {
        if(!(initialized_ = initialise(data_)))
            releaseData();
    }

    delete data_;
    data_ = nullptr;

    return initialized_;
}

bool ResourceBase::ready()
{
    if(initialized_)
    {
        return true;
    }

    // If the data has been read from the disk, we can initialise it
    else if(dataReady_)
    {
        initialized_ = initialise(data_);
        dataReady_ = false;

        if(initialized_)
        {
            emit initialized();
        }

        else
        {
            releaseData();
        }

        delete data_;
        data_ = nullptr;
    }

    return initialized_;
}

bool ResourceBase::managed() const
{
    return despatcher_ != nullptr;
}

ResourceDespatcher* ResourceBase::despatcher()
{
    assert(despatcher_);
    return despatcher_;
}

const QString& ResourceBase::name() const
{
    return name_;
}

void ResourceBase::release()
{
    if(!name_.isEmpty())
    {
        qDebug() << __FUNCTION__ << "Releasing resource:" << name_;
    }

    initialized_ = false;
    dataReady_ = false;

    if(data_ != nullptr)
    {
        delete data_;
        data_ = nullptr;
    }

    emit released();

    // Call implementation
    releaseData();
}

void ResourceBase::queryFilesDebug(QStringList& files) const
{
    files.push_back(name_);
}