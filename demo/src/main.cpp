//
//  Author   : Matti Määttä
//  Summary  : 
//

#include <QGuiApplication>
#include <QSurfaceFormat>
#include <QQuickItem>
#include <QScreen>

#include "sceneview.h"
#include "demoapplication.h"
#include "uicontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Create OpenGL context
    QSurfaceFormat format;
    format.setVersion(4, 2);
    format.setSamples(2);
    format.setProfile(QSurfaceFormat::CoreProfile);

#ifdef _DEBUG
    format.setOption(QSurfaceFormat::DebugContext);
#endif

    // Create application
    DemoApplication demo(format);

    Engine::Ui::SceneView view;
    view.setSource(QUrl("qrc:/demoui/main.qml"));

    // Disable VSync.
    /*QSurfaceFormat viewFormat = view.format();
    viewFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
    view.setFormat(viewFormat);*/

    demo.setView(&view);

    UiController ui;
    ui.setRootObject(view.rootObject());
    ui.setView(&view);

    demo.setUiController(&ui);

#ifdef _DEBUG
    // Move the window to second monitor for debugging convenience
    view.setGeometry(app.screens().last()->availableGeometry());
#endif

    view.show();
    
    return app.exec();
}