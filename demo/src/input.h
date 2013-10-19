#ifndef INPUT_H
#define INPUT_H

#include <QObject>
#include <QPoint>
#include <QMap>

class QWindow;

class Input : public QObject
{
    Q_OBJECT

public:
    enum { KEY_MOUSE_RIGHT = -100 };

    Input(QWindow* parent);
    ~Input();

    // Tells if a key is being pressed
    bool keyDown(int keyCode) const;

    // Mouse delta relative to last frame
    QPoint mouseDelta() const;

    // Wheel delta relative to last frame
    int wheelDelta() const;

    void endFrame();

    // Relayed events
    void wheelEvent(int delta);
    void keyEvent(int key, bool state);
    void mouseEvent(int key, bool state);

private:
    QPoint lastMouse_;
    int wheelDelta_;

    QMap<int, bool> keyMap_;
};

#endif // INPUT_H