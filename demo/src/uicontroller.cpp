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
    connect(this, SIGNAL(updateFrametime(QVariant)), stats, SLOT(updateFrametime(QVariant)));
}

void UiController::setView(QQuickView* view)
{
    connect(view, &QQuickView::frameSwapped, this, &UiController::frameSwapped);
}

void UiController::frameSwapped()
{
    emit updateFrametime(timer_.restart());
}