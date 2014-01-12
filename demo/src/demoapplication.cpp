#include "demoapplication.h"

#include "demopresenter.h"
#include "sceneview.h"

#include <QQuickItem>

DemoApplication::DemoApplication(const QSurfaceFormat& format)
    : Application(format), controller_(nullptr)
{
}

DemoApplication::~DemoApplication()
{
}

void DemoApplication::setUiController(UiController* controller)
{
    controller_ = controller;
}

Engine::Ui::ScenePresenter* DemoApplication::createPresenter()
{
    DemoPresenter* presenter = new DemoPresenter;
    view()->setEventListener(presenter->inputListener());

    QObject* tonemap = view()->rootObject()->findChild<QObject*>("tonemap");
    if(tonemap != nullptr)
    {
        QObject::connect(tonemap, SIGNAL(valueChanged(QString, QVariant)),
            presenter, SLOT(tonemapAttributeChanged(QString, QVariant)));
    }

    QObject* general = view()->rootObject()->findChild<QObject*>("general");
    if(general != nullptr)
    {
        QObject::connect(general, SIGNAL(valueChanged(QString, QVariant)),
            presenter, SLOT(generalAttributeChanged(QString, QVariant)));
    }

    presenter->setUiController(controller_);

    return presenter;
}