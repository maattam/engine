//
//  Author   : Matti Määttä
//  Summary  : 
//

#ifndef DEMOAPPLICATION_H
#define DEMOAPPLICATION_H

#include "application.h"

class UiController;

class DemoApplication : public Engine::Ui::Application
{
    Q_OBJECT

public:
    explicit DemoApplication(const QSurfaceFormat& format);
    virtual ~DemoApplication();

    void setUiController(UiController* controller);

protected:
    virtual Engine::Ui::ScenePresenter* createPresenter();

private:
    UiController* controller_;
};

#endif // DEMOAPPLICATION_H