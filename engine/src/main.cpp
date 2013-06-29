#include <QtGui/QGuiApplication>

#include "sceneview.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    SceneView view;
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();
    
    return app.exec();
}
