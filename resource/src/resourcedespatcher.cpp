#include "resourcedespatcher.h"

#include "resource.h"

#include <QFileSystemWatcher>
#include <QFile>
#include <QDebug>

using namespace Engine;

ResourceDespatcher::ResourceDespatcher(QObject* parent)
    : QObject(parent), loader_(nullptr), watcher_(nullptr)
{
    loader_ = new ResourceLoader();
    loader_->moveToThread(&loadThread_);

    connect(this, &ResourceDespatcher::loadResources, loader_, &ResourceLoader::run);
    connect(loader_, &ResourceLoader::resourceLoaded, this, &ResourceDespatcher::resourceLoaded, Qt::QueuedConnection);
    loadThread_.start();

    emit loadResources();

#ifdef _DEBUG
    watcher_ = new QFileSystemWatcher(this);
    connect(watcher_, &QFileSystemWatcher::fileChanged, this, &ResourceDespatcher::fileChanged);
#endif
}

ResourceDespatcher::~ResourceDespatcher()
{
    clear();

    delete loader_;
    loader_ = nullptr;
}

void ResourceDespatcher::clear()
{
    loader_->stop();
    loadThread_.quit();
    loadThread_.wait();

    resources_.clear();
}

int ResourceDespatcher::numManaged() const
{
    int count = 0;

    for(auto it = resources_.begin(); it != resources_.end(); ++it)
    {
        if(!it->expired())
            count++;
    }

    return count;
}

void ResourceDespatcher::fileChanged(const QString& path)
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

void ResourceDespatcher::watchResource(const std::shared_ptr<ResourceBase>& resource)
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

void ResourceDespatcher::resourceLoaded(const QString& id)
{
    auto result = resources_.find(id);
    if(result != resources_.end() && !result->expired())
    {
        auto handle = result->lock();
        handle->ready();
    }
}