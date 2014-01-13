#include "rendertimewatcher.h"

#include "renderstage.h"

#include <QOpenGLTimeMonitor>

RenderTimeWatcher::RenderTimeWatcher()
    : QObject(), frameCaptured_(true)
{
}

RenderTimeWatcher::~RenderTimeWatcher()
{
}

void RenderTimeWatcher::addRenderStage(const QString& name, Engine::RenderStage* stage)
{
    if(stage == nullptr || name.isEmpty())
    {
        return;
    }

    connect(stage, &Engine::RenderStage::stageFinished, this, &RenderTimeWatcher::renderStageFinished);
    stages_ << name;
}

void RenderTimeWatcher::addNamedStage(const QString& name)
{
    if(name.isEmpty())
    {
        return;
    }

    stages_ << name;
}

void RenderTimeWatcher::clearStages()
{
    stages_.clear();
    averages_.clear();

    if(monitor_.isCreated())
    {
        monitor_.destroy();
    }
}

bool RenderTimeWatcher::create()
{
    if(monitor_.isCreated())
    {
        monitor_.destroy();
    }

    if(stages_.count() < 1)
    {
        return false;
    }

    monitor_.setSampleCount(stages_.count() + 1);
    averages_.resize(stages_.count());

    return monitor_.create();
}

void RenderTimeWatcher::endFrame()
{
    if(frameCaptured_)
    {
        frameCaptured_ = false;
    }

    if(monitor_.isResultAvailable())
    {
        QVector<GLuint64> intervals = monitor_.waitForIntervals();
        for(int i = 0; i < intervals.size(); ++i)
        {
            averages_[i] << intervals[i];

            double average = averages_[i] * 10e-7;
            emit timeUpdated(stages_[i], average, "ms");
        }

        monitor_.reset();
        frameCaptured_ = true;
    }
}

void RenderTimeWatcher::setTimestamp()
{
    renderStageFinished();
}

void RenderTimeWatcher::renderStageFinished()
{
    if(frameCaptured_)
    {
        monitor_.recordSample();
    }
}