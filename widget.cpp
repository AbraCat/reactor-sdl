#include "widget.h"
#include "sdl-adapter.h"

#include <iostream>
#include <cassert>



State* state = nullptr;

State::State() : focused(nullptr)
{
    //
}




CoordSystem::CoordSystem(Vector centre, double xScale, double yScale) :
    centre(centre), xScale(xScale), yScale(yScale)
{
    //
}

Vector CoordSystem::getAbsCentre() { return centre; }

Vector CoordSystem::tranformToBaseCoord(Vector coord)
{
    Vector objectCoord, absCentre = getAbsCentre();
    objectCoord.x = coord.x * xScale + absCentre.x;
    objectCoord.y = coord.y * yScale + absCentre.y;

    return objectCoord;
}

Vector CoordSystem::baseToTransformCoord(Vector coord)
{
    Vector planeCoord, absCentre = getAbsCentre();
    planeCoord.x = (coord.x - absCentre.x) * 1.0 / xScale;
    planeCoord.y = (coord.y - absCentre.y) * 1.0 / yScale;

    return planeCoord;
}

void CoordSystem::transform(Vector centre, double xScale, double yScale)
{
    this->centre = centre;
    this->xScale = xScale;
    this->yScale = yScale;
}

void CoordSystem::move(Vector newPos) { transform(newPos, xScale, yScale); }
void CoordSystem::rescale(double new_scale_x, double new_scale_y) { transform(centre, new_scale_x, new_scale_y); }
void CoordSystem::rescale(double new_scale_x, double new_scale_y, Vector point)
{
    Vector pos_change = point - centre;
    move(point);

    double scale_change_x = new_scale_x / xScale, scale_change_y = new_scale_y / yScale;
    rescale(new_scale_x, new_scale_y);

    pos_change.x *= scale_change_x;
    pos_change.y *= scale_change_y;
    move(point - pos_change);
}




Texture::Texture(Widget* w) : CoordSystem({0, 0}, 1, 1)
{
    this->w = w;
    this->visible_in = w;
}

void Texture::setVisibleIn(Widget* w) { this->visible_in = w; }

Vector Texture::getAbsCentre() { return centre + w->getAbsTL(); }

void Texture::rescaleCentre(double new_scale_x, double new_scale_y)
{
    rescale(new_scale_x, new_scale_y, w->wh * 0.5);
}

void Texture::paint()
{   
    if (w->w_border_visible)
    {
        setColor(whiteV);
        drawRect(w->absTL, w->absTL + w->wh, 0);
    }

    for (ColFixedVec rect: rects) paintRect(rect);
    for (ColPoint p : points) paintPoint(p);
    for (ColFixedVec l: lines) paintLine(l);
    for (ColCircle c: circles) paintCircle(c);
    for (ColPolygon pol: polygons) paintPolygon(pol);
    paintText();
}

void Texture::paintText()
{
    if (text.size() != 0 && visible_in->inAbsRect(w->getAbsTL() + w->wh * 0.5))
        putText(text, w->getAbsTL(), w->getAbsTL() + w->wh);
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

void Texture::paintPoint(ColPoint p)
{
    Vector absP = tranformToBaseCoord(p.p);
    if (visible_in->inAbsRect(absP))
    {
        setColor(p.col);
        drawPoint(absP);
    }
}

void Texture::paintLine(ColFixedVec l)
{
    Vector absP1 = tranformToBaseCoord(l.vec.p1), absP2 = tranformToBaseCoord(l.vec.p2);
    FixedVec ans;

    if (clipLine({absP1, absP2}, {visible_in->absTL, visible_in->absTL + visible_in->wh}, &ans));
    {
        setColor(l.col);
        drawLine(ans.p1, ans.p2);
    }
}

void Texture::paintRect(ColFixedVec rect)
{
    if (rect.fill == 0) assert("should pass rect border as 4 lines" && 0);

    Vector absP1 = tranformToBaseCoord(rect.vec.p1), absP2 = tranformToBaseCoord(rect.vec.p2);

    FixedVec intersection;
    if (rectIntersection({absP1, absP2},
        {visible_in->absTL, visible_in->absTL + visible_in->wh}, &intersection))
    {
        setColor(rect.col);
        drawRect(intersection.p1, intersection.p2, rect.fill);
    }

    // setColor(rect.col);
    // drawRect(absP1, absP2, rect.fill);
}

void Texture::paintCircle(ColCircle c)
{
    Vector absC = tranformToBaseCoord(c.centre);
    if (visible_in->inAbsRect(absC))
    {
        setColor(c.col);
        drawCircle(absC, c.r * xScale, c.fill);
    }
}

void Texture::paintPolygon(ColPolygon pol)
{
    if (pol.fill == 0) assert("polygon border unimplemented" && 0);

    std::vector<Vector> absPoints;
    for (Vector p: pol.ps)
        absPoints.push_back(tranformToBaseCoord(p));

    setColor(pol.col);
    fillConvexPolygon(absPoints);
}

void Texture::addText(std::string text) { this->text = text; }
void Texture::addPoint(Vector p, Vector color) { points.push_back({p, color}); }

void Texture::addLine(FixedVec line, Vector color) { lines.push_back({line, color, 0}); }
void Texture::addRect(FixedVec rect, Vector color, bool fill)
{
    if (fill) rects.push_back({rect, color, fill});
    else
    {
        Vector bl(rect.p1.x, rect.p2.y), tr(rect.p2.x, rect.p1.y);

        addLine({rect.p1, bl}, color);
        addLine({rect.p1, tr}, color);
        addLine({rect.p2, bl}, color);
        addLine({rect.p2, tr}, color);
    }
}

void Texture::addCircle(Vector centre, Vector col, double r, bool fill)
{
    circles.push_back({centre, col, r, fill});
}

void Texture::addPolygon(std::vector<Vector> points, Vector color, bool fill)
{
    polygons.push_back({points, color, fill});
}

void Texture::addVector(FixedVec v, Vector color)
{
    const double arrowCoeff = 0.2;

    Vector vec = fixedToFree(v);
    Vector ort1 = {vec.y, -vec.x}, ort2 = {-vec.y, vec.x};
    Vector e1 = (ort1 - vec) * arrowCoeff, e2 = (ort2 - vec) * arrowCoeff;

    addLine(v, color);
    addPolygon({v.p2, v.p2 + e2, v.p2 + e1}, color, 1);
}




PixelTexture::PixelTexture(Widget* w) : Texture(w)
{
    pix = new Vector[w->width * w->height];
}

PixelTexture::~PixelTexture() { delete pix; }

void PixelTexture::paint()
{
    for (int y = 0; y < w->height; ++y)
    {
        for (int x = 0; x < w->width; ++x)
        {
            setColor(getPix(x, y));
            drawPoint(Vector(x, y) + w->getAbsTL());
        }
    }

    Texture::paint();
}

void PixelTexture::setPix(int x, int y, Vector col) { pix[y * w->width + x] = col; }
Vector PixelTexture::getPix(int x, int y) { return pix[y * w->width + x]; }




Widget::Widget(Vector tl, Vector br, Widget* parent)
{
    this->tl = tl;
    this->br = br;
    width = br.x - tl.x;
    height = br.y - tl.y;
    this->wh = Vector(width, height);
    // print(wh);
    // printf("widget\n");
    // print(tl);
    // print(br);

    w_border_visible = 0;
    t_border_visible = 0;
    fill_rect = 0;
    fill_rect_color = Vector(0, 0, 0);

    this->draggable = 0;
    this->dragged = 0;
    this->dragTL = this->dragBR = {};

    this->t = new Texture(this);

    this->parent = parent;
    if (parent != nullptr)
    {
        this->absTL = parent->absTL + tl;
        parent->addWidget(this);
    }
    else
        this->absTL = Vector(0, 0);
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

Vector Widget::propagatedAbsTL() { return absTL; }

void Widget::propagateAbsPos()
{
    for (Widget *w: children)
    {
        // w->absTL = this->absTL + w->tl;
        w->absTL = this->propagatedAbsTL() + w->tl;
        w->propagateAbsPos();
    }
}

void Widget::setDraggable(Vector dragTL, Vector dragBR)
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

void Widget::setPixelTexture(bool pixel_texture)
{
    if (t != nullptr) delete t;

    if (pixel_texture)
    {
        t = new PixelTexture(this);
        return;
    }

    t = new Texture(this);
}

void Widget::setWidgetBorderVisible(bool visible) { w_border_visible = visible; }
void Widget::setTextureBorderVisible(bool visible) { t_border_visible = visible; }

void Widget::setFillRect(bool fill, Vector color)
{
    fill_rect = fill;
    fill_rect_color = color;
}

Vector Widget::getAbsTL() { return this->absTL; }

bool Widget::inRect(Vector point)
{
    return point.x >= tl.x && point.x <= br.x && point.y >= tl.y && point.y <= br.y;
}

bool Widget::inAbsRect(Vector point)
{
    return point.x >= absTL.x && point.x <= absTL.x + wh.x && point.y >= absTL.y && point.y <= absTL.y + wh.y;
}

void Widget::resize(Vector newtl, Vector newbr)
{
    Vector TLchange = newtl - tl;
    this->absTL = newtl - tl + absTL;

    this->tl = newtl;
    this->br = newbr;

    width = br.x - tl.x;
    height = br.y - tl.y;
    wh = Vector(width, height);

    propagateAbsPos();
}

void Widget::movePos(Vector newtl)
{
    resize(newtl, br + newtl - tl);
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

void Widget::addWidget(Widget* child)
{
    assert(child->parent == this);
    children.push_back(child);
}

void Widget::clearChildren()
{
    for (Widget* w: children)
        delete w;

    children.clear();
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
        dragMouse = Vector(e->x, e->y) - parent->absTL;
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
        Vector newDragMouse = Vector(e->x, e->y) - parent->absTL;
        Vector change = newDragMouse - dragMouse;

        Vector newTL = tl + change, newBR = br + change;
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

bool Widget::keyboardEvent(KeyboardEvent* e)
{
    return 0;
}

bool Widget::onIdle(IdleEvent* e)
{
    return 0;
}


WContainer::WContainer(Widget* parent, Vector tl, Vector br, int nChildren, bool vertical, double list_length)
    : Widget(tl, br, parent)
{
    // setTextureBorderVisible(1);
    setWidgetBorderVisible(1);
    this->vertical = vertical;
    this->nChildren = nChildren;
    this->padding = 0;

    // vertical
    if (list_length == 0) list_length = height;
    this->list_length = list_length;
    scroll_frac = 0;

    if (vertical)
    {
        this->childHeight = (list_length - padding * (nChildren + 1)) / nChildren;
        this->childWidth = width - 2 * padding;
    }
    else
    {
        this->childWidth = (width - padding * (nChildren + 1)) / nChildren;
        this->childHeight = height - 2 * padding;
    }

    block_children_mouse_down = false;
}

void WContainer::addWidget(Widget* w)
{
    assert(children.size() < nChildren);
    int nChild = children.size();
    Widget::addWidget(w);

    w->t->setVisibleIn(this);

    if (vertical) w->resize(Vector(padding, padding * (nChild + 1) + childHeight * nChild), 
        Vector(wh.x - padding, (padding + childHeight) * (nChild + 1)));

    else w->resize(Vector(padding * (nChild + 1) + childWidth * nChild, padding), 
        Vector((padding + childWidth) * (nChild + 1), wh.y - padding));
}

Vector WContainer::propagatedAbsTL()
{
    // vertical
    return Vector(absTL.x, absTL.y - scroll_frac * (list_length - height));
}

void WContainer::paint()
{
    Widget::paint();
}

void WContainer::scroll(double frac)
{
    scroll_frac = frac;
    propagateAbsPos();
    paint();
}

bool WContainer::handleEvent(Event* e)
{
    MouseEvent* press_evt = dynamic_cast<MouseEvent*>(e);
    if (block_children_mouse_down && press_evt != NULL && press_evt->type == MOUSE_DOWN && !inAbsRect(Vector(press_evt->x, press_evt->y)))
        return e->dispatch(this);

    for (Widget* w: children)
        if (w->handleEvent(e)) return 1;
    return e->dispatch(this);
}

void WContainer::blockChildrenMouseDown(bool block) { block_children_mouse_down = block; }



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