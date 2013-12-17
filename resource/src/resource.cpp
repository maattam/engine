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
    : data_(nullptr), despatcher_(nullptr), dataReady_(false), initialized_(false),
    released_(false)
{
}

ResourceBase::ResourceBase(const QString& name, InitialisePolicy policy)
    : data_(nullptr), despatcher_(nullptr), dataReady_(false), initialized_(false),
    released_(false), name_(name), policy_(policy)
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
        {
            qDebug() << "Failed to initialise resource:" << name_;
            releaseData();
        }
    }

    delete data_;
    data_ = nullptr;

    return initialized_;
}

bool ResourceBase::ready()
{
    // Non-managed resource state is considered ready
    if(!managed() || initialized_)
    {
        return true;
    }

    // If the data has been read from file, we can initialise it
    else if(dataReady_)
    {
        assert(data_);
        initialized_ = initialise(data_);

        if(initialized_)
        {
            emit initialized(name_);
        }

        else
        {
            qDebug() << "Failed to initialise resource:" << name_;
            releaseData();
        }

        dataReady_ = false;     // Mark data as invalid

        // Delete cached data
        delete data_;
        data_ = nullptr;

        released_ = false;
    }

    return initialized_;
}

bool ResourceBase::managed() const
{
    return despatcher_ != nullptr;
}

ResourceDespatcher* ResourceBase::despatcher()
{
    return despatcher_;
}

const QString& ResourceBase::name() const
{
    return name_;
}

ResourceBase::InitialisePolicy ResourceBase::initialisePolicy() const
{
    return policy_;
}

void ResourceBase::release()
{
    if(released_)
    {
        return;
    }

    if(!name_.isEmpty())
    {
        qDebug() << __FUNCTION__ << "Releasing resource:" << name_;
    }

    initialized_ = false;
    released_ = true;

    emit released(name_);

    // Call implementation
    releaseData();
}

void ResourceBase::queryFilesDebug(QStringList& files) const
{
    files.push_back(name_);
}