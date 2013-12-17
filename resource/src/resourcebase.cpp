#include "resourcebase.h"
#include "resourcedata.h"

#include <QDebug>

using namespace Engine;

ResourceBase::ResourceBase()
    : despatcher_(nullptr), initialized_(false), released_(true)
{
}

ResourceBase::ResourceBase(const QString& name, InitialisePolicy policy)
    : despatcher_(nullptr), initialized_(false), released_(true), name_(name), policy_(policy)
{
}

ResourceBase::~ResourceBase()
{
}

bool ResourceBase::initialiseFromData(const ResourceDataPtr& data)
{
    data_ = data;

    if(!managed())
    {
        return initialiseResource();
    }

    if(policy_ == QUEUED)
    {
        return ready();
    }

    return true;
}

bool ResourceBase::initialiseResource()
{
    if(!data_)
    {
        return false;
    }

    release();

    initialized_ = initialise(data_.get());
    if(!initialized_)
    {
        qWarning() << "Failed to initialise resource:" << name_;
        releaseResource();
    }

    else
    {
        emit initialized(name_);
        released_ = false;
    }

    data_.reset();
    return initialized_;
}

bool ResourceBase::load(const QString& fileName)
{
    // We don't want the data to be reloaded if the object is being managed by a despatcher
    if(managed())
    {
        qWarning() << __FUNCTION__ << "Attempted to invoke load on a managed resource.";
        return false;
    }

    data_ = createData();
    if(!data_->load(fileName))
    {
        qWarning() << "Failed to load" << fileName;

        data_.reset();
        return false;
    }

    return initialiseResource();
}

bool ResourceBase::ready()
{
    if(initialized_)
    {
        return true;
    }

    // If the data has been read from file, we can initialise it
    else if(data_ != nullptr)
    {
        return initialiseResource();
    }

    return false;
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
    releaseResource();
}

bool ResourceBase::managed() const
{
    return despatcher_ != nullptr;
}

ResourceDespatcher* ResourceBase::despatcher()
{
    return despatcher_;
}

void ResourceBase::setDespatcher(ResourceDespatcher* despatcher)
{
    despatcher_ = despatcher;
}

const QString& ResourceBase::name() const
{
    return name_;
}

ResourceBase::InitialisePolicy ResourceBase::initialisePolicy() const
{
    return policy_;
}

void ResourceBase::queryFilesDebug(QStringList& files) const
{
    files.push_back(name_);
}