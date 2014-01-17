//
//  Author   : Matti Määttä
//  Summary  : 
//

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

    auto result = watchList_.find(path);
    if(result == watchList_.end())
    {
        qWarning() << "WatchList does not contain entry" << path;
        return;
    }

    bool allExpired = true;
    for(WeakResourcePtr& handle : *result)
    {
        if(!handle.expired())
        {
            QFile file;
            file.setFileName(path);

            if(file.exists())
            {
                auto resource = handle.lock();
                resource->release();

                pushResource(resource->name(), resource);
            }

            allExpired = false;
        }
    }

    if(allExpired)
    {
        watcher_->removePath(path);
        watchList_.remove(path);
    }
}

void WeakResourceDespatcher::watchResource(const ResourcePtr& resource, const ResourceDataPtr& data)
{
#ifdef _DEBUG
    QStringList files;
    
    files << resource->name();
    files << data->queryFilesDebug();

    watcher_->addPaths(files);

    for(const QString& file : files)
    {
        QList<WeakResourcePtr>& list = watchList_[file];
        bool found = false;

        for(WeakResourcePtr& handle : list)
        {
            ResourcePtr ptr = handle.lock();
            if(ptr && ptr->name() == resource->name())
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            //qDebug() << __FUNCTION__ << "Trigger:" << file;
            list << resource;
        }
    }
#endif
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
    if(handle->initialiseFromData(data))
    {
        watchResource(handle, data);
    }
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