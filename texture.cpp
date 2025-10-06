#include "texture.h"



CoordSystem::CoordSystem(IntVec centre, double xScale, double yScale) :
    centre(centre), xScale(xScale), yScale(yScale)
{
    //
}



Texture::Texture(Widget* w) : CoordSystem(w->getAbsTL(), 1, 1)
{
    this->w = w;
}

void Texture::paint()
{
    for (ColPoint p : points)
    {
        IntVec absP = tranformToBaseCoord(p.p);
        if (w->inAbsRect(absP))
        {
            setColor(p.col);
            drawPoint(absP);
        }
    }
}

void Texture::clear()
{
    points.clear();
    lines.clear();
    vectors.clear();
    rects.clear();
    circles.clear();
    polygons.clear();
}

void Texture::addPoint(Vector p, Vector color) { points.push_back({p, color}); }

void Texture::addLine(FixedVec line, Vector color) { lines.push_back({line, color, 0}); }
void Texture::addVector(FixedVec v, Vector color) { vectors.push_back({v, color, 0}); }
void Texture::addRect(FixedVec rect, Vector color, bool fill) { rects.push_back({rect, color, fill}); }

void Texture::addCircle(Vector centre, Vector col, double r, bool fill) { circles.push_back({centre, col, r, fill}); }
void Texture::addPolygon(std::vector<Vector> points, Vector color, bool fill) { polygons.push_back({}); }