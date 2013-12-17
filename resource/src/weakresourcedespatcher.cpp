#include "weakresourcedespatcher.h"

#include "resource.h"
#include "resourceloader.h"

#include <QFileSystemWatcher>
#include <QFile>
#include <QDebug>

using namespace Engine;

WeakResourceDespatcher::WeakResourceDespatcher(QObject* parent)
    : ResourceDespatcher(parent), loader_(nullptr), watcher_(nullptr)
{
    loader_ = new ResourceLoader();
    loader_->moveToThread(&loadThread_);

    connect(this, &WeakResourceDespatcher::loadResources, loader_, &ResourceLoader::run);
    connect(loader_, &ResourceLoader::resourceLoaded, this, &WeakResourceDespatcher::resourceLoaded, Qt::QueuedConnection);
    loadThread_.start();

    emit loadResources();

#ifdef _DEBUG
    watcher_ = new QFileSystemWatcher(this);
    connect(watcher_, &QFileSystemWatcher::fileChanged, this, &WeakResourceDespatcher::fileChanged);
#endif
}

WeakResourceDespatcher::~WeakResourceDespatcher()
{
    clear();

    delete loader_;
    loader_ = nullptr;
}

void WeakResourceDespatcher::clear()
{
    loader_->stop();
    loadThread_.quit();
    loadThread_.wait();

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
            loader_->pushResource(handle);
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

void WeakResourceDespatcher::resourceLoaded(const QString& id)
{
    WeakResourcePtr result = findResource(id);
    
    auto handle = result.lock();
    if(handle != nullptr)
    {
        handle->ready();
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
    watchResource(resource);

    loader_->pushResource(resource);
}