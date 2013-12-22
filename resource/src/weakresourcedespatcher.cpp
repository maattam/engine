#include "weakresourcedespatcher.h"

#include "resource.h"
#include "resourceloader.h"

#include <QFileSystemWatcher>
#include <QFile>
#include <QDebug>

using namespace Engine;

WeakResourceDespatcher::WeakResourceDespatcher(unsigned int threadCount, QObject* parent)
    : ResourceDespatcher(parent), watcher_(nullptr)
{
    qRegisterMetaType<ResourcePtr>("ResourcePtr");
    qRegisterMetaType<ResourceDataPtr>("ResourceDataPtr");

    threadPool_.setMaxThreadCount(threadCount);
    qDebug() << "Despatcher thread pool size:" << threadCount;

#ifdef _DEBUG
    watcher_ = new QFileSystemWatcher(this);
    connect(watcher_, &QFileSystemWatcher::fileChanged, this, &WeakResourceDespatcher::fileChanged);
#endif
}

WeakResourceDespatcher::~WeakResourceDespatcher()
{
    clear();
}

void WeakResourceDespatcher::clear()
{
    // Wait for threads to finish.
    threadPool_.waitForDone();

    resources_.clear();
}

int WeakResourceDespatcher::numManaged() const
{
    int count = 0;

    for(auto it = resources_.begin(); it != resources_.end(); ++it)
    {
        if(!it->expired())
            count++;
    }

    return count;
}

void WeakResourceDespatcher::fileChanged(const QString& path)
{
    qDebug() << __FUNCTION__ << path;

    auto result = resources_.find(path);
    if(result != resources_.end() && !result->expired())
    {
        QFile file;
        file.setFileName(path);

        if(file.exists())
        {
            auto handle = result->lock();
            handle->release();

            pushResource(handle->name(), handle);
        }
    }

    else
    {
        watcher_->removePath(path);
    }
}

void WeakResourceDespatcher::watchResource(const ResourcePtr& resource)
{
    if(watcher_ != nullptr && resource != nullptr)
    {
        QStringList files;
        resource->queryFilesDebug(files);
        watcher_->addPaths(files);

        for(auto it = files.begin(); it != files.end(); ++it)
        {
            if(resources_.count(*it) == 0)
            {
                qDebug() << __FUNCTION__ << "Trigger:" << *it;
                resources_[*it] = resource;
            }
        }
    }
}

void WeakResourceDespatcher::resourceLoaded(QString id, ResourceDataPtr data)
{
    auto handle = findResource(id).lock();
    if(handle == nullptr)
    {
        qWarning() << __FUNCTION__ << "Resource loaded but handle is missing:" << id;
        return;
    }

    // If the resource should be initialised on next frame, signal despatcher
    handle->initialiseFromData(data);
}

WeakResourceDespatcher::WeakResourcePtr WeakResourceDespatcher::findResource(const QString& fileName)
{
    WeakResourcePtr weakPtr;

    auto result = resources_.find(fileName);
    if(result != resources_.end())
    {
        weakPtr = result.value();
    }

    return weakPtr;
}

void WeakResourceDespatcher::insertResource(const QString& fileName, const ResourcePtr& resource)
{
    resources_.insert(fileName, resource);

    resource->setDespatcher(this);
    watchResource(resource);

    pushResource(fileName, resource);
}

void WeakResourceDespatcher::loadResource(ResourcePtr resource)
{
    insertResource(resource->name(), resource);
}

void WeakResourceDespatcher::pushResource(const QString& fileName, const ResourcePtr& resource)
{
    ResourceLoader* loader = new ResourceLoader(*resource, fileName, *this);

    connect(loader, &ResourceLoader::resourceLoaded, this, &WeakResourceDespatcher::resourceLoaded);
    loader->setAutoDelete(true);

    threadPool_.start(loader);
}