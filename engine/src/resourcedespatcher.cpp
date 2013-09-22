#include "resourcedespatcher.h"

using namespace Engine;

ResourceDespatcher::ResourceDespatcher()
    : QObject(), loader_(nullptr)
{
    loader_ = new ResourceLoader();
    loader_->moveToThread(&loadThread_);

    connect(this, &ResourceDespatcher::loadResources, loader_, &ResourceLoader::run);
    loadThread_.start();

    emit loadResources();
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