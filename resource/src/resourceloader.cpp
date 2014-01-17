//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "resourceloader.h"

#include "resourcedata.h"
#include "resourcebase.h"

#include <QDebug>
#include <QThread>

using namespace Engine;

ResourceLoader::ResourceLoader(ResourceBase& resource, const QString& fileName, ResourceDespatcher& despatcher, QObject* parent)
    : QObject(parent), QRunnable(), target_(despatcher), fileName_(fileName)
{
    data_ = resource.createData();
}

void ResourceLoader::run()
{
    proxy_.setTarget(&target_);
    data_->setDespatcher(&proxy_);

    qDebug() << "Loading:" << fileName_;

    if(data_->load(fileName_))
    {
        emit resourceLoaded(fileName_, data_);
    }

    else
    {
        qWarning() << "Failed to load:" << fileName_;
        data_.reset();
    }
}