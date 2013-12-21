#ifndef INPUTEVENTLISTENER_H
#define INPUTEVENTLISTENER_H

#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

namespace Engine { namespace Ui {

class InputEventListener : public QObject
{
    Q_OBJECT

public:
    explicit InputEventListener(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    virtual void keyPressEvent(QKeyEvent ev) = 0;
    virtual void keyReleaseEvent(QKeyEvent ev) = 0;

    virtual void mouseMoveEvent(QMouseEvent ev) = 0;
    virtual void mousePressEvent(QMouseEvent ev) = 0;
    virtual void mouseReleaseEvent(QMouseEvent ev) = 0;

    virtual void wheelEvent(QWheelEvent ev) = 0;
};

}}

#endif // INPUTEVENTLISTENER_H