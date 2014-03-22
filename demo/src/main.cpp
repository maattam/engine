//
//  Author   : Matti Määttä
//  Summary  : 
//

#include <QGuiApplication>
#include <QSurfaceFormat>
#include <QScreen>

#include "sceneview.h"
#include "qmlapplication.h"
#include "uicontroller.h"
#include "scenefactory.h"

// Demo scenes
#include "basicscene.h"
#include "sponzascene.h"
#include "lightscene.h"
#include "gameoflife.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Create OpenGL context
    QSurfaceFormat format;
    format.setVersion(4, 2);
    format.setSamples(1);
    format.setProfile(QSurfaceFormat::CoreProfile);

#ifdef _DEBUG
    format.setOption(QSurfaceFormat::DebugContext);
#endif

    // Create application
    Engine::Ui::QmlApplication demo(format);
    demo.profile(true);

    // Create controller for communicating between QmlPresenter and Qt Quick
    Engine::Ui::UiController ui;
    demo.setUiController(&ui);

    // Create window
    Engine::Ui::SceneView view;
    demo.setView(&view);
    view.setSource(QUrl("qrc:/demoui/main.qml"));

    // Register scene types
    Engine::Ui::SceneFactory factory;
    demo.setSceneFactory(&factory, "Sponza");

    factory.registerScene<BasicScene>("Shittyboxes");
    factory.registerScene<SponzaScene>("Sponza");
    factory.registerScene<LightScene>("Lights");
    factory.registerScene<GameOfLife>("Game of life");

    // Disable VSync.
    /*QSurfaceFormat viewFormat = view.format();
    viewFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
    view.setFormat(viewFormat);*/

#ifdef _DEBUG
    // Move the window to second monitor for debugging convenience
    view.setGeometry(app.screens().last()->availableGeometry());
#endif

    view.show();
    
    return app.exec();
}