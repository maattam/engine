//
//  Author   : Matti Määttä
//  Summary  : Application which creates ScenePresenter that listens events from
//             Qt Quick interface.
//

#ifndef QMLAPPLICATION_H
#define QMLAPPLICATION_H

#include "application.h"

#include <QString>

namespace Engine { namespace Ui {

class UiController;
class SceneFactory;

class QmlApplication : public Application
{
    Q_OBJECT

public:
    explicit QmlApplication(const QSurfaceFormat& format);
    virtual ~QmlApplication();

    void setUiController(UiController* controller);
    void setSceneFactory(SceneFactory* factory, const QString& startScene);

protected:
    virtual ScenePresenter* createPresenter();

private:
    UiController* controller_;
    SceneFactory* factory_;
    QString scene_;
};

}}

#endif // QMLAPPLICATION_H