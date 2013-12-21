#include <QGuiApplication>
#include <QSurfaceFormat>
#include <QScreen>

#include "rendertargetitem.h"
#include "sceneview.h"
#include "demoapplication.h"

using namespace Engine::Ui;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Register custom QML classes
    qmlRegisterType<RenderTargetItem>("Engine", 1, 0, "RenderTarget");

    // Create OpenGL context
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);

#ifdef _DEBUG
    format.setOption(QSurfaceFormat::DebugContext);
#endif

    // Create application
    DemoApplication demo(format);

    SceneView view;
    view.setSource(QUrl("qrc:/main.qml"));

    demo.setView(&view);

#ifdef _DEBUG
    // Move the window to second monitor for debugging convenience
    view.setGeometry(app.screens().last()->availableGeometry());
#endif

    view.show();
    
    return app.exec();
}