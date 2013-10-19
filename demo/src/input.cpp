#include "input.h"

#include <QWindow>
#include <QWheelEvent>
#include <QEvent>
#include <QCursor>
#include <QDebug>

Input::Input(QWindow* parent) : QObject(parent)
{
    endFrame();
}

Input::~Input()
{
}

bool Input::keyDown(int keyCode) const
{
    auto iter = keyMap_.find(keyCode);
    if(iter != keyMap_.end() && iter.value())
    {
        return true;
    }

    return false;
}

QPoint Input::mouseDelta() const
{
    return lastMouse_ - QCursor::pos();
}

int Input::wheelDelta() const
{
    return wheelDelta_;
}

void Input::wheelEvent(int delta)
{
    wheelDelta_ = delta;
}

void Input::keyEvent(int key, bool state)
{
    keyMap_[key] = state;
}

void Input::mouseEvent(int key, bool state)
{
    keyMap_[key] = state;

    if(state)
    {
        lastMouse_ = QCursor::pos();
    }
}

void Input::endFrame()
{
    wheelDelta_ = 0;

    if(keyDown(KEY_MOUSE_RIGHT))
    {
        QCursor::setPos(lastMouse_);
    }
}