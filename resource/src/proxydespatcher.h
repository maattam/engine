#ifndef PROXYDESPATCHER_H
#define PROXYDESPATCHER_H

#include "resourcedespatcher.h"

#include <QHash>

namespace Engine {

class ProxyDespatcher : public ResourceDespatcher
{
    Q_OBJECT

public:
    explicit ProxyDespatcher(QObject* parent = nullptr);
    virtual ~ProxyDespatcher();

    // Clears the record; doesn't delete managed objects
    virtual void clear();

    // Counts managed objects; expensive
    virtual int numManaged() const;

    // Precondition: despatcher != nullptr
    void setTarget(ResourceDespatcher* despatcher);

signals:
    void moveResource(ResourcePtr resource);

public slots:
    // Can be used to move resources between despatchers.
    // The resource is loaded by the target despatcher and ownership is copied.
    virtual void loadResource(ResourcePtr resource);

protected:
    virtual WeakResourcePtr findResource(const QString& fileName);
    virtual void insertResource(const QString& fileName, const ResourcePtr& resource);

private:
    QHash<QString, WeakResourcePtr> resources_;
    ResourceDespatcher* target_;

    ProxyDespatcher(const ProxyDespatcher&);
    ProxyDespatcher& operator=(const ProxyDespatcher&);
};

}

#endif // PROXYDESPATCHER_H