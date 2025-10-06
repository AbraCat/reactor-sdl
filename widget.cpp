#include "widget.h"
#include "sdl-adapter.h"

#include <iostream>
#include <cassert>







CoordSystem::CoordSystem(IntVec centre, double xScale, double yScale) :
    centre(centre), xScale(xScale), yScale(yScale)
{
    //
}

IntVec CoordSystem::tranformToBaseCoord(Vector coord)
{
    IntVec objectCoord;
    objectCoord.x = coord.x * xScale + centre.x;
    objectCoord.y = coord.y * yScale + centre.y;
    return objectCoord;
}

Vector CoordSystem::baseToTransformCoord(IntVec coord)
{
    Vector planeCoord;
    planeCoord.x = (coord.x - centre.x) * 1.0 / xScale;
    planeCoord.y = (coord.y - centre.y) * 1.0 / yScale;
    return planeCoord;
}



Texture::Texture(Widget* w) : CoordSystem(w->getAbsTL(), 1, 1)
{
    this->w = w;
}

void Texture::paint()
{
    const bool always_draw = 1;
    
    if (w->w_border_visible)
    {
        setColor(whiteV);
        drawRect(w->absTL, w->absTL + w->wh, 0);
    }

    for (ColFixedVec l: rects)
    {
        IntVec absP1 = tranformToBaseCoord(l.vec.p1), absP2 = tranformToBaseCoord(l.vec.p2);
        if (always_draw || w->inAbsRect(absP1) && w->inAbsRect(absP2))
        {
            setColor(l.col);
            drawRect(absP1, absP2, l.fill);
        }
    }

    for (ColPoint p : points)
    {
        IntVec absP = tranformToBaseCoord(p.p);
        if (always_draw || w->inAbsRect(absP))
        {
            setColor(p.col);
            drawPoint(absP);
        }
    }

    for (ColFixedVec l: lines)
    {
        IntVec absP1 = tranformToBaseCoord(l.vec.p1), absP2 = tranformToBaseCoord(l.vec.p2);
        if (always_draw || w->inAbsRect(absP1) && w->inAbsRect(absP2))
        {
            setColor(l.col);
            drawLine(absP1, absP2);
        }
    }

    for (ColCircle c: circles)
    {
        IntVec absC = tranformToBaseCoord(c.centre);
        if (always_draw || w->inAbsRect(absC))
        {
            setColor(c.col);
            drawCircle(absC, c.r * xScale, c.fill);
        }
    }

    for (ColPolygon pol: polygons)
    {
        if (pol.fill == 0) assert("unimplemented" && 0);

        std::vector<IntVec> absPoints;
        for (Vector p: pol.ps)
            absPoints.push_back(tranformToBaseCoord(p));

        setColor(pol.col);
        fillConvexPolygon(absPoints);
    }
}

void Texture::paintRec()
{
    paint();
    for (Widget* child: w->children)
        child->t->paintRec();
}

void Texture::clear()
{
    points.clear();
    lines.clear();
    rects.clear();
    circles.clear();
    polygons.clear();
}

void Texture::transform(IntVec centre, double xScale, double yScale)
{
    this->centre = centre;
    this->xScale = xScale;
    this->yScale = yScale;
}

void Texture::move(IntVec change) { transform(centre + change, xScale, yScale); }
void Texture::rescale(double scale_change) { transform(centre, xScale * scale_change, yScale * scale_change); }

void Texture::addPoint(Vector p, Vector color) { points.push_back({p, color}); }

void Texture::addLine(FixedVec line, Vector color) { lines.push_back({line, color, 0}); }
void Texture::addRect(FixedVec rect, Vector color, bool fill) { rects.push_back({rect, color, fill}); }

void Texture::addCircle(Vector centre, Vector col, double r, bool fill) { circles.push_back({centre, col, r, fill}); }
void Texture::addPolygon(std::vector<Vector> points, Vector color, bool fill)
    { polygons.push_back({points, color, fill}); }

void addVector(Texture* t, FixedVec v, Vector color)
{
    const double arrowCoeff = 0.2;

    Vector vec = fixedToFree(v);
    Vector ort1 = {vec.y, -vec.x}, ort2 = {-vec.y, vec.x};
    Vector e1 = (ort1 - vec) * arrowCoeff, e2 = (ort2 - vec) * arrowCoeff;

    t->addLine(v, color);
    t->addPolygon({v.p2, v.p2 + e2, v.p2 + e1}, color, 1);
}



Widget::Widget(IntVec tl, IntVec br, Widget* parent)
{
    this->tl = tl;
    this->br = br;
    width = br.x - tl.x;
    height = br.y - tl.y;
    this->wh = IntVec(width, height);

    t_border_visible = 0;
    fill_rect = 0;
    fill_rect_color = Vector(0, 0, 0);

    this->draggable = 0;
    this->dragged = 0;
    this->dragTL = this->dragBR = {};

    this->parent = parent;
    if (parent != nullptr)
    {
        this->absTL = parent->absTL + tl;
        parent->addWidget(this);
    }
    else
        this->absTL = IntVec(0, 0);

    this->t = new Texture(this);
}

Widget::~Widget()
{
    for (Widget* w: children)
        delete w;
    delete t;
}

void Widget::paint()
{
    t->clear();

    if (fill_rect) t->addRect({{0, 0}, wh}, {0, 0, 0}, 1);
    if (t_border_visible) t->addRect({{0, 0}, wh}, {255, 255, 255}, 0);
}

void Widget::setDraggable(IntVec dragTL, IntVec dragBR)
{
    assert(parent != nullptr);
    this->draggable = 1;

    if ((dragBR - dragTL).x == 0 && (dragBR - dragTL).y == 0)
    {
        this->dragTL = {0, 0};
        this->dragBR = parent->wh;
    }
    else
    {
        this->dragTL = dragTL;
        this->dragBR = dragBR;
    }
}

void Widget::setWidgetBorderVisible(bool visible)
{
    w_border_visible = visible;
}

void Widget::setTextureBorderVisible(bool visible)
{
    t_border_visible = visible;
}

void Widget::setFillRect(bool fill, Vector color)
{
    fill_rect = fill;
    fill_rect_color = color;
}

IntVec Widget::getAbsTL() { return this->absTL; }

bool Widget::inRect(IntVec point)
{
    return point.x >= tl.x && point.x <= br.x && point.y >= tl.y && point.y <= br.y;
}

bool Widget::inAbsRect(IntVec point)
{
    return point.x >= absTL.x && point.x <= absTL.x + wh.x && point.y >= absTL.y && point.y <= absTL.y + wh.y;
}

void Widget::resize(IntVec newtl, IntVec newbr)
{
    IntVec TLchange = newtl - tl;
    this->absTL = newtl - tl + absTL;
    if (t != nullptr) t->centre += TLchange;

    this->tl = newtl;
    this->br = newbr;

    width = br.x - tl.x;
    height = br.y - tl.y;
    wh = IntVec(width, height);
}

void Widget::movePos(IntVec newtl)
{
    // propagateAbsTL() ?

    IntVec change = newtl - tl;
    tl = newtl;
    br += change;

    absTL += change;
    if (t != nullptr) t->centre += change;
}

void Widget::drawWidgetRect(bool fill, Vector color)
{
    t->addRect({{0, 0}, wh}, color, fill);
}

void Widget::drawRec()
{
    this->paint();
    for (Widget* w: children)
        w->drawRec();
}

void Widget::addChild(Widget* widget)
{
    assert(widget->parent == nullptr);

    children.push_back(widget);
    // widget->setParent(this);
}

void Widget::addWidget(Widget* child)
{
    assert(child->parent == this);
    children.push_back(child);
}

bool Widget::handleEvent(Event* e)
{
    for (Widget* w: children)
        if (w->handleEvent(e)) return 1;
    return e->dispatch(this);
}

bool Widget::mousePressEvent(MouseEvent* e)
{
    if (!inAbsRect({e->x, e->y})) return 0;
    if (!draggable) return 0;
    assert(parent != nullptr);

    if (!dragged)
    {
        dragged = 1;
        dragMouse = IntVec(e->x, e->y) - parent->absTL;
        return 1;
    }

    return 0;
}

bool Widget::mouseMoveEvent(MouseEvent* e)
{
    if (!draggable) return 0;
    assert(parent != nullptr);

    if (dragged)
    {
        IntVec newDragMouse = IntVec(e->x, e->y) - parent->absTL;
        IntVec change = newDragMouse - dragMouse;

        IntVec newTL = tl + change, newBR = br + change;
        if (newTL.x < dragTL.x || newBR.x > dragBR.x) change.x = 0;
        if (newTL.y < dragTL.y || newBR.y > dragBR.y) change.y = 0;

        movePos(tl + change);
        dragMouse = newDragMouse;
        return 1;
    }

    return 0;
}

bool Widget::mouseReleaseEvent(MouseEvent* e)
{
    if (!draggable) return 0;

    if (dragged)
    {
        dragged = 0;
        return 1;
    }
    return 0;
}

bool Widget::onIdle(IdleEvent* e)
{
    return 0;
}


WContainer::WContainer(Widget* parent, IntVec tl, IntVec br, int nChildren, bool vertical) : Widget(tl, br, parent)
{
    setTextureBorderVisible(1);
    this->vertical = vertical;
    this->nChildren = nChildren;
    this->padding = 10;

    if (vertical)
    {
        this->childHeight = (height - padding * (nChildren + 1)) / nChildren;
        this->childWidth = width - 2 * padding;
    }
    else
    {
        this->childWidth = (width - padding * (nChildren + 1)) / nChildren;
        this->childHeight = height - 2 * padding;
    }
}

void WContainer::addWidget(Widget* w)
{
    assert(children.size() < nChildren);
    int nChild = children.size();
    Widget::addWidget(w);

    if (vertical) w->resize(IntVec(padding, padding * (nChild + 1) + childHeight * nChild), 
        IntVec(wh.x - padding, (padding + childHeight) * (nChild + 1)));

    else w->resize(IntVec(padding * (nChild + 1) + childWidth * nChild, padding), 
        IntVec((padding + childWidth) * (nChild + 1), wh.y - padding));
}

void WContainer::paint()
{
    Widget::paint();
}



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