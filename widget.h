#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL.h>

#include <vector>
#include <string>

#include "myvector.h"

class Texture;
class Widget;
class Event;
class MouseEvent;
class IdleEvent;



class CoordSystem
{
public:
    CoordSystem(Vector centre, double xScale, double yScale);

    Vector tranformToBaseCoord(Vector coord);
    Vector baseToTransformCoord(Vector coord);

    virtual void transform(Vector centre, double xScale, double yScale);
    void move(Vector newPos);
    void rescale(double new_scale_x, double new_scale_y);
    void rescale(double new_scale_x, double new_scale_y, Vector point);

    virtual Vector getAbsCentre();

// protected:
    Vector centre;
    double xScale, yScale;
};



struct ColPoint
{
    Vector p, col;
};

struct ColFixedVec
{
    FixedVec vec;
    Vector col;
    bool fill;
};

struct ColCircle
{
    Vector centre, col;
    double r;
    bool fill;
};

struct ColPolygon
{
    std::vector<Vector> ps;
    Vector col;
    bool fill;
};

class Texture : public CoordSystem
{
public:
    Texture(Widget* w);
    void paint();
    void paintRec();
    void clear();

    virtual Vector getAbsCentre() override;
    void rescaleCentre(double new_scale_x, double new_scale_y);

    void addText(std::string text);
    void addPoint(Vector p, Vector color);
    void addLine(FixedVec line, Vector color);

    void addRect(FixedVec rect, Vector color, bool fill);
    void addCircle(Vector centre, Vector col, double r, bool fill);
    void addPolygon(std::vector<Vector> points, Vector color, bool fill);
    void addVector(FixedVec v, Vector color);

    void paintPoint(ColPoint p);
    void paintLine(ColFixedVec line);
    void paintRect(ColFixedVec rect);
    void paintCircle(ColCircle cirlce);
    void paintPolygon(ColPolygon polygon);

// protected:
    Widget* w;

    std::string text;
    std::vector<ColPoint> points;
    std::vector<ColFixedVec> lines, rects;
    std::vector<ColCircle> circles;
    std::vector<ColPolygon> polygons;
};





/*
coordinate systems:

absTL - absolute
tl, br - parent
everything else - this (if not said otherwise)
*/
class Widget
{
public:
    Widget(Vector tl, Vector br, Widget* parent = nullptr);
    ~Widget();
    virtual void addWidget(Widget* child);
    void clearChildren();

    void setWidgetBorderVisible(bool visible);
    void setTextureBorderVisible(bool visible);
    void setFillRect(bool fill, Vector color = Vector(0, 0, 0));
    void setDraggable(Vector dragTL = Vector(), Vector dragBR = Vector());

    virtual Vector getAbsTL();
    virtual Vector propagatedAbsTL();
    void propagateAbsPos();

    void drawWidgetRect(bool fill, Vector color = Vector(0, 0, 0));
    bool inRect(Vector point);
    bool inAbsRect(Vector point);

    virtual void resize(Vector newtl, Vector newbr);
    virtual void movePos(Vector newtl);

    void drawRec();
    virtual void paint();

    virtual bool handleEvent(Event* e);
    virtual bool onIdle(IdleEvent* e);
    virtual bool mousePressEvent(MouseEvent* e);
    virtual bool mouseMoveEvent(MouseEvent* e);
    virtual bool mouseReleaseEvent(MouseEvent* e);
// protected:
    Texture *t;

    Vector tl, br, wh, absTL;
    int width, height;

    bool draggable, dragged;
    Vector dragTL, dragBR, dragMouse; // relative to parent

    Vector fill_rect_color;
    bool w_border_visible, t_border_visible, fill_rect;

    Widget *parent;
    std::vector<Widget*> children;
};

class WContainer : public Widget
{
public:
    WContainer(Widget* parent, Vector tl, Vector br, int nChildren, bool vertical, double list_length = 0);
    virtual void paint() override;

    virtual void addWidget(Widget* widget) override;
    virtual Vector propagatedAbsTL() override;
    virtual bool handleEvent(Event* e) override;
    void scroll(double frac);

private:
    bool vertical;
    int nChildren, padding, childWidth, childHeight;

    double scroll_frac, list_length;
};



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

#endif // WIDGET_H