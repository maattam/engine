#ifndef RESOURCEDELOADER_H
#define RESOURCEDELOADER_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <atomic>
#include <memory>

namespace Engine {

class Resource;

class ResourceLoader : public QObject
{
    Q_OBJECT

public:
    typedef std::shared_ptr<Resource> ResourcePtr;

    ResourceLoader();
    ~ResourceLoader();

    void stop();

    void pushResource(const ResourcePtr& resource);

public slots:
    void run();

private:
    QQueue<ResourcePtr> loadQueue_;
    QMutex mutex_;
    QWaitCondition notEmpty_;
    std::atomic<bool> running_;
};

}

#endif  // RESOURCEDELOADER_H