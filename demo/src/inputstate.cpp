#include "inputstate.h"

InputState::InputState(QObject* parent)
    : InputEventListener(parent), wheelDelta_(0)
{
}

InputState::~InputState()
{
}

bool InputState::keyDown(int keyCode) const
{
    auto iter = keyMap_.find(keyCode);
    if(iter != keyMap_.end() && iter.value())
    {
        return true;
    }

    return false;
}

const QPoint& InputState::mousePos() const
{
    return lastMouse_;
}

int InputState::wheelDelta()
{
    int temp = wheelDelta_;
    wheelDelta_ = 0;

    return temp;
}

void InputState::setKey(int keyCode, bool state)
{
    keyMap_[keyCode] = state;
}

void InputState::keyPressEvent(QKeyEvent ev)
{
    keyMap_[ev.key()] = true;
}

void InputState::keyReleaseEvent(QKeyEvent ev)
{
    keyMap_[ev.key()] = false;
}

void InputState::mouseMoveEvent(QMouseEvent ev)
{
    lastMouse_ = ev.pos();
}

void InputState::mousePressEvent(QMouseEvent ev)
{
    if(ev.button() == Qt::RightButton)
    {
        keyMap_[KEY_MOUSE_RIGHT] = true;
    }
}

void InputState::mouseReleaseEvent(QMouseEvent ev)
{
    if(ev.button() == Qt::RightButton)
    {
        keyMap_[KEY_MOUSE_RIGHT] = false;
    }
}

void InputState::wheelEvent(QWheelEvent ev)
{
    wheelDelta_ += ev.delta();
}