#ifndef RENDERTIMEWATCHER_H
#define RENDERTIMEWATCHER_H

#include <QObject>
#include <QString>
#include <QOpenGLTimeMonitor>
#include <QStringList>

namespace Engine {
    class RenderStage;
}

class RenderTimeWatcher : public QObject
{
    Q_OBJECT

public:
    RenderTimeWatcher();
    virtual ~RenderTimeWatcher();

    void addRenderStage(const QString& name, Engine::RenderStage* stage);
    void addNamedStage(const QString& name);

    void clearStages();

    // Called after render stages have been added
    bool create();

    void endFrame();

    void setTimestamp();

signals:
    void timeUpdated(QString name, qreal time, QString unit);

public slots:
    void renderStageFinished();

private:
    QOpenGLTimeMonitor monitor_;

    QStringList stages_;
    bool frameCaptured_;
};

#endif // RENDERTIMEWATCHER_H