#include <QGuiApplication>

#include "sceneview.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    SceneView view;
    view.resize(800, 600);

#ifdef _DEBUG
    view.setScreen(app.screens().last());
#endif

    view.show();
    
    return app.exec();
}
