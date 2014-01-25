//
//  Author   : Matti Määttä
//  Summary  : UiController updates various UI elements such as fps.
//

#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include "movingaverage.h"

#include <QObject>
#include <QElapsedTimer>
#include <QVariant>
#include <QStringList>

class QQuickView;
class QQuickItem;

namespace Engine { namespace Ui {

class QmlPresenter;

class UiController : public QObject
{
    Q_OBJECT

public:
    explicit UiController(QObject* parent = nullptr);

    void setRootObject(QQuickItem* root);
    void setView(QQuickView* view);
    void setQmlPresenter(QmlPresenter* presenter);

    void setSceneList(const QStringList& scenes, const QString& current);

signals:
    void updateValue(QVariant name, QVariant value, QVariant unit);
    void updateScenes(QVariant list, QVariant current);
    void clearValues();

public slots:
    void frameSwapped();
    void watchValue(QString name, qreal value, QString unit);
    void clearWatchList();

private:
    QElapsedTimer timer_;

    MovingAverage<qint64, double, 10> frameTime_;
};

}}

#endif // demoui/main.qml