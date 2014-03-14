//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "qmlapplication.h"

#include "qmlpresenter.h"
#include "sceneview.h"
#include "uicontroller.h"
#include "scenefactory.h"

#include <QQuickItem>

using namespace Engine::Ui;

QmlApplication::QmlApplication(const QSurfaceFormat& format)
    : Application(format), controller_(nullptr), factory_(nullptr), profile_(false)
{
}

QmlApplication::~QmlApplication()
{
}

void QmlApplication::profile(bool value)
{
    profile_ = value;
}

void QmlApplication::setUiController(UiController* controller)
{
    controller_ = controller;
}

void QmlApplication::setSceneFactory(SceneFactory* factory, const QString& startScene)
{
    factory_ = factory;
    scene_ = startScene;
}

Engine::Ui::ScenePresenter* QmlApplication::createPresenter()
{
    QmlPresenter* presenter = new QmlPresenter(profile_);
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

    if(controller_ != nullptr)
    {
        controller_->setQmlPresenter(presenter);
        controller_->setRootObject(view()->rootObject());
        controller_->setView(view());

        // Update list of registered scenes to UI. UI is guaranteed to be created at this point.
        controller_->setSceneList(factory_->sceneTypes(), scene_);
    }

    presenter->setSceneFactory(factory_);
    presenter->setScene(scene_);

    return presenter;
}