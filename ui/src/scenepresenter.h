//
//  Author   : Matti Määttä
//  Summary  : ScenePresenter manages scene drawing in the rendering thread.
//

#ifndef SCENEPRESENTER_H
#define SCENEPRESENTER_H

#include <QObject>
#include <QString>
#include <QSize>

namespace Engine { namespace Ui {

class RendererContext;
class SceneFactory;

class ScenePresenter : public QObject
{
    Q_OBJECT

public:
    explicit ScenePresenter(QObject* parent = nullptr) : QObject(parent) {}

    virtual void setContext(RendererContext* context) = 0;
    virtual void setSceneFactory(SceneFactory* factory) = 0;

public slots:
    // Called after the view has finished rendering the scene graph.
    // Precondition: viewInitialized and viewSizeChanged received.
    virtual void renderScene() = 0;

    // Called when the render target size changes.
    virtual void viewSizeChanged(QSize size) = 0;

    // Called after OpenGL context has been created and OpenGL calls are safe to be done.
    virtual void initialize() = 0;

    // Called when scene should be changed. Associated SceneFactory should contain method
    // to create given scene.
    virtual void setScene(QString scene) = 0;
};

}}

#endif // SCENEPRESENTER_H