// UiController updates various UI elements such as fps.

#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <QObject>
#include <QElapsedTimer>
#include <QVariant>

class QQuickView;
class QQuickItem;

class UiController : public QObject
{
    Q_OBJECT

public:
    explicit UiController(QObject* parent = nullptr);

    void setRootObject(QQuickItem* root);
    void setView(QQuickView* view);

signals:
    void updateFrametime(QVariant frametime);

public slots:
    void frameSwapped();

private:
    QElapsedTimer timer_;
};

#endif // demoui/main.qml