#ifndef DEMOAPPLICATION_H
#define DEMOAPPLICATION_H

#include "application.h"

class DemoApplication : public Engine::Ui::Application
{
    Q_OBJECT

public:
    explicit DemoApplication(const QSurfaceFormat& format);
    virtual ~DemoApplication();

protected:
    virtual Engine::Ui::ScenePresenter* createPresenter();
};

#endif // DEMOAPPLICATION_H