//
//  Author   : Matti M‰‰tt‰
//  Summary  : 
//

#include "uicontroller.h"

#include <QQuickView>
#include <QQuickItem>

UiController::UiController(QObject* parent)
    : QObject(parent)
{
}

void UiController::setRootObject(QQuickItem* root)
{
    QQuickItem* stats = root->findChild<QQuickItem*>("stats");
    connect(this, SIGNAL(updateValue(QVariant, QVariant, QVariant)), stats, SLOT(watchValue(QVariant, QVariant, QVariant)));
    connect(this, SIGNAL(clearValues()), stats, SLOT(clearValues()));
}

void UiController::setView(QQuickView* view)
{
    connect(view, &QQuickView::frameSwapped, this, &UiController::frameSwapped);
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