#ifndef MY_EVENT_H
#define MY_EVENT_H

class Widget;



class Event
{
public:
    virtual bool dispatch(Widget* w) = 0;
};

enum MouseEnum
{
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_MOVE
};

class MouseEvent : public Event
{
public:
    MouseEvent(MouseEnum type, int x, int y);
    virtual bool dispatch(Widget* w) override;

    int x, y;
    MouseEnum type;
};

class IdleEvent : public Event
{
    virtual bool dispatch(Widget* w) override;
};

class KeyboardEvent : public Event
{
public:
    KeyboardEvent(char key);
    virtual bool dispatch(Widget* w) override;

    char key;
};



#endif // MY_EVENT_H