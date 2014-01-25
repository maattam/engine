//
//  Author   : Matti M‰‰tt‰
//  Summary  : 
//

#include "uicontroller.h"

#include "qmlpresenter.h"

#include <QQuickView>
#include <QQuickItem>

using namespace Engine::Ui;

UiController::UiController(QObject* parent)
    : QObject(parent)
{
}

void UiController::setRootObject(QQuickItem* root)
{
    QQuickItem* stats = root->findChild<QQuickItem*>("stats");
    if(stats != nullptr)
    {
        connect(this, SIGNAL(updateValue(QVariant, QVariant, QVariant)), stats, SLOT(watchValue(QVariant, QVariant, QVariant)));
        connect(this, SIGNAL(clearValues()), stats, SLOT(clearValues()));
    }

    QQuickItem* general = root->findChild<QQuickItem*>("general");
    if(general != nullptr)
    {
        connect(this, SIGNAL(updateScenes(QVariant,QVariant)), general, SLOT(setScenes(QVariant,QVariant)));
    }
}

void UiController::setView(QQuickView* view)
{
    connect(view, &QQuickView::frameSwapped, this, &UiController::frameSwapped);
}

void UiController::setQmlPresenter(QmlPresenter* presenter)
{
    connect(presenter, &QmlPresenter::watchValue, this, &UiController::watchValue);
    connect(presenter, &QmlPresenter::clearWatchList, this, &UiController::clearWatchList);
}

void UiController::setSceneList(const QStringList& scenes, const QString& current)
{
    emit updateScenes(QVariant::fromValue(scenes), QVariant::fromValue(current));
}

void UiController::frameSwapped()
{
    frameTime_ << timer_.nsecsElapsed();
    timer_.restart();

    double frameAverage = frameTime_ * 10e-7;
    watchValue("FPS", 1000.0 / frameAverage, "");
    watchValue("Frame time", frameAverage, "ms");
}

void UiController::watchValue(QString name, qreal value, QString unit)
{
    emit updateValue(name, value, unit);
}

void UiController::clearWatchList()
{
    emit clearValues();
}