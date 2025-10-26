#include "texture.h"
#include "sdl-adapter.h"
#include "widget.h"
#include "assert.h"

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