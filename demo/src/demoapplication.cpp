#include "demoapplication.h"

#include "demopresenter.h"
#include "sceneview.h"

DemoApplication::DemoApplication(const QSurfaceFormat& format)
    : Application(format)
{
}

DemoApplication::~DemoApplication()
{
}

Engine::Ui::ScenePresenter* DemoApplication::createPresenter()
{
    DemoPresenter* presenter = new DemoPresenter;
    view()->setEventListener(presenter->inputListener());

    return presenter;
}