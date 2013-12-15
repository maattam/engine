#include <QGuiApplication>
#include <QSurfaceFormat>
#include <QScreen>

#include "sceneview.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Create OpenGL 4.2 context
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

#ifdef _DEBUG
    format.setOption(QSurfaceFormat::DebugContext);
#endif

    SceneView view(format);
    view.resize(800, 600);

#ifdef _DEBUG
    // Move the window to second monitor for debugging convenience
    view.setGeometry(app.screens().last()->geometry());
#endif

    view.show();
    
    return app.exec();
}