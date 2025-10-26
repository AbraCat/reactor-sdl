#include "event.h"
#include "widget.h"


MouseEvent::MouseEvent(MouseEnum type, int x, int y)
{
    this->type = type;
    this->x = x;
    this->y = y;
}

bool MouseEvent::dispatch(Widget* w)
{
    switch (type)
    {
        case MOUSE_DOWN: return w->mousePressEvent(this);
        case MOUSE_UP: return w->mouseReleaseEvent(this);
        case MOUSE_MOVE: return w->mouseMoveEvent(this);
    }
    return 0;
}

bool IdleEvent::dispatch(Widget* w)
{
    return w->onIdle(this);
}

KeyboardEvent::KeyboardEvent(char key) : key(key)
{
    //
}

bool KeyboardEvent::dispatch(Widget* w)
{
    return w->keyboardEvent(this);
}