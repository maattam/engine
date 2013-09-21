#include "resourceloader.h"

#include "resource.h"

#include <QDebug>
#include <QThread>

using namespace Engine;

ResourceLoader::ResourceLoader()
    : QObject(), running_(false)
{
    qDebug() << "ResourceLoader::ResourceLoader(): Thread: " << thread()->currentThreadId();
}

ResourceLoader::~ResourceLoader()
{
}

void ResourceLoader::run()
{
    running_ = true;

    while(running_)
    {
        mutex_.lock();
        if(loadQueue_.empty())
        {
            notEmpty_.wait(&mutex_);
        }

        if(!running_)
        {
            break;
        }

        ResourcePtr resource = loadQueue_.dequeue();
        mutex_.unlock();

        if(!resource->loadData(resource->name()))
        {
            qDebug() << "ResourceLoader::run(): Failed to load resource: " << resource->name();
        }

        else
        {
            resource->dataReady_ = true;
            qDebug() << "ResourceLoader::run(): Loaded resource: " << resource->name();
        }
    }
}

void ResourceLoader::pushResource(const ResourcePtr& resource)
{
    mutex_.lock();
    loadQueue_.enqueue(resource);
    notEmpty_.wakeAll();
    mutex_.unlock();
}

void ResourceLoader::stop()
{
    running_ = false;
    notEmpty_.wakeAll();
}