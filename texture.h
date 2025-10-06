#ifndef MY_TEXTURE_MANAGER
#define MY_TEXTURE_MANAGER

#include "sdl-adapter.h"

#error

class CoordSystem
{
public:
    CoordSystem(IntVec centre, double xScale, double yScale);

    IntVec tranformToBaseCoord(Vector coord);
    Vector baseToTransformCoord(IntVec coord);

// protected:
    IntVec centre;
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
    void clear();

    void addPoint(Vector p, Vector color);
    void addLine(FixedVec line, Vector color);
    void addVector(FixedVec v, Vector color);

    void addRect(FixedVec rect, Vector color, bool fill);
    void addCircle(Vector centre, Vector col, double r, bool fill);
    void addPolygon(std::vector<Vector> points, Vector color, bool fill);

// protected:
    Widget* w;

    // point line vector rect(2) circle(2) polygon(2)
    std::vector<ColPoint> points;
    std::vector<ColFixedVec> lines, vectors, rects;
    std::vector<ColCircle> circles;
    std::vector<ColPolygon> polygons;
};

#endif // MY_TEXTURE_MANAGER