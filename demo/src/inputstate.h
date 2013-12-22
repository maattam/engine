#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include "inputeventlistener.h"

#include <QPoint>
#include <QMap>

class InputState : public Engine::Ui::InputEventListener
{
    Q_OBJECT

public:
    enum { KEY_MOUSE_RIGHT = -100 };

    explicit InputState(QObject* parent = nullptr);
    virtual ~InputState();

    // Tells if a key is being pressed
    bool keyDown(int keyCode) const;

    // Tells current mouse position
    const QPoint& mousePos() const;

    // Wheel delta relative to last last call.
    int wheelDelta();

    void setKey(int keyCode, bool state);

public slots:
    virtual void keyPressEvent(QKeyEvent ev);
    virtual void keyReleaseEvent(QKeyEvent ev);

    virtual void mouseMoveEvent(QMouseEvent ev);
    virtual void mousePressEvent(QMouseEvent ev);
    virtual void mouseReleaseEvent(QMouseEvent ev);

    virtual void wheelEvent(QWheelEvent ev);

private:
    QPoint lastMouse_;
    int wheelDelta_;

    QMap<int, bool> keyMap_;
};

#endif // INPUTSTATE_H