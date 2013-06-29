#include <QtGui/QGuiApplication>

#include "sceneview.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    SceneView view;
    view.show();
    
    return app.exec();
}
