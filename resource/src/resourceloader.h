#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <QRunnable>
#include <QObject>
#include <QString>

#include <memory>

#include "resourcedata.h"
#include "proxydespatcher.h"

namespace Engine {

class ResourceData;
class ResourceBase;

class ResourceLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
    typedef std::shared_ptr<ResourceData> ResourceDataPtr;

    explicit ResourceLoader(ResourceBase& resource, const QString& fileName, ResourceDespatcher& despatcher,
        QObject* parent = nullptr);

    virtual void run();

signals:
    void resourceLoaded(QString name, ResourceDataPtr data);

private:
    ProxyDespatcher proxy_;
    ResourceDataPtr data_;
    ResourceDespatcher& target_;
    QString fileName_;
};

}

#endif // RESOURCELOADER_H