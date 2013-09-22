#include "resourceloader.h"

#include "resource.h"

#include <QDebug>
#include <QThread>

using namespace Engine;

ResourceLoader::ResourceLoader()
    : QObject(), running_(false)
{
}

ResourceLoader::~ResourceLoader()
{
}

void ResourceLoader::run()
{
    qDebug() << __FUNCTION__ << "Thread: " << thread()->currentThreadId();
    running_ = true;

    while(running_)
    {
        // Acquire the queue mutex and process the topmost resource
        mutex_.lock();
        if(loadQueue_.empty())
        {
            // If the queue is empty, we suspend the thread and wait for the producer
            // thread to push more resources
            notEmpty_.wait(&mutex_);
        }

        if(!running_)
        {
            break;
        }

        // We copy the resource so we don't have to block the producer thread
        // while loading the data from storage
        ResourcePtr resource = loadQueue_.dequeue();
        mutex_.unlock();

        if(!resource->loadData(resource->name()))
        {
            qWarning() << __FUNCTION__ << "Failed to load resource: " << resource->name();
        }

        else
        {
            resource->dataReady_ = true;
            qDebug() << __FUNCTION__ << "Loaded resource: " << resource->name();
        }
    }
}

void ResourceLoader::pushResource(const ResourcePtr& resource)
{
    // Acquire mutex and enqueue the new resource
    mutex_.lock();
    loadQueue_.enqueue(resource);

    // Wake the consumer thread if it's suspended
    notEmpty_.wakeAll();
    mutex_.unlock();
}

void ResourceLoader::stop()
{
    running_ = false;
    notEmpty_.wakeAll();
}