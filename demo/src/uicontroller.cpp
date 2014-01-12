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
}

void UiController::setView(QQuickView* view)
{
    connect(view, &QQuickView::frameSwapped, this, &UiController::frameSwapped);
}

void UiController::frameSwapped()
{
    double elapsed = timer_.nsecsElapsed() * 10e-7;
    timer_.restart();

    watchValue("FPS", 1000.0 / elapsed, "");
    watchValue("Frame time", elapsed, "ms");
}

void UiController::watchValue(QString name, qreal value, QString unit)
{
    emit updateValue(name, value, unit);
}