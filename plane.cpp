#include "plane.h"
#include "sdl-adapter.h"

#include <cmath>

const int stdNPoints = 100;
const int axisWidth = 3, cutsWidth = 1, borderWidth = 1, cutsLength = 5, pointSize = 3;

const double arrowCoeff = 0.1;



BasePlane::BasePlane(Widget* parent, IntVec tl, IntVec br, IntVec centre,
    double xScale, double cutStepX, double yScale, double cutStepY) : Widget(tl, br, parent),
    xScale(xScale), yScale(yScale), cutStepX(cutStepX), cutStepY(cutStepY), centre(centre)
{
    setBorderVisible(1);
    setFillRect(1);

    axisVisible = 1;
}

void BasePlane::setAxisVisible(bool axisVisible)
{
    this->axisVisible = axisVisible;
}

void BasePlane::drawCuts()
{
    setColor({255, 255, 255});

    for (int x = centre.x; x < br.x; x += xScale * cutStepX)
        drawLine({x, centre.y - cutsLength}, {x, centre.y + cutsLength});
    for (int x = centre.x; x > tl.x; x -= xScale * cutStepX)
        drawLine({x, centre.y - cutsLength}, {x, centre.y + cutsLength});
    for (int y = centre.y; y < br.y; y += yScale * cutStepY)
        drawLine({centre.x - cutsLength, y}, {centre.x + cutsLength, y});
    for (int y = centre.y; y > tl.y; y -= yScale * cutStepY)
        drawLine({centre.x - cutsLength, y}, {centre.x + cutsLength, y});
}

void BasePlane::paint()
{
    Widget::paint();

    if (axisVisible)
    {
        setColor({255, 255, 255});
        drawLine({tl.x, centre.y}, {br.x, centre.y});
        drawLine({centre.x, tl.y}, {centre.x, br.y});

        drawCuts();
    }
}

IntVec BasePlane::planeToObjectCoord(Vector coord)
{
    IntVec objectCoord;
    objectCoord.x = coord.x * xScale + centre.x;
    objectCoord.y = coord.y * yScale + centre.y;
    return objectCoord;
}

Vector BasePlane::objectToPlaneCoord(IntVec coord)
{
    Vector planeCoord;
    planeCoord.x = (coord.x - centre.x) * 1.0 / xScale;
    planeCoord.y = (coord.y - centre.y) * 1.0 / yScale;
    return planeCoord;
}


Plane::Plane(Widget* parent, IntVec tl, IntVec br, IntVec centre,
    double xScale, double cutStepX, double yScale, double cutStepY) :
    BasePlane(parent, tl, br, centre, xScale, cutStepX, yScale, cutStepY)
{
    //
}

void Plane::paint()
{
    BasePlane::paint();

    for (int nPoint = 0; nPoint < points.size(); ++nPoint)
    {
        Vector point = points[nPoint], color = pointColors[nPoint];
        IntVec planePoint = planeToObjectCoord(point);

        setColor(color);
        drawPoint(planePoint);
    }

    for (int nVec = 0; nVec < vectors.size(); ++nVec)
    {
        FixedVector vec = vectors[nVec];
        Vector color = vectorColors[nVec];

        setColor(color);
        drawVector(vec, color);
    }
}

void Plane::addPoint(Vector point, Vector color)
{
    points.push_back(point);
    pointColors.push_back(color);
}

void Plane::addVector(FixedVector vec, Vector color)
{
    vectors.push_back(vec);
    vectorColors.push_back(color);
}

void Plane::drawVector(FixedVector v, Vector color)
{
    setColor(color);
    IntVec p1 = planeToObjectCoord(v.p1), p2 = planeToObjectCoord(v.p2);
    drawLine({p1.x, p1.y}, {p2.x, p2.y});

    Vector vec = fixedToFree(v);
    Vector ort1 = {vec.y, -vec.x}, ort2 = {-vec.y, vec.x};
    Vector e1 = (ort1 - vec) * arrowCoeff, e2 = (ort2 - vec) * arrowCoeff;
    IntVec e1Int = planeToObjectCoord(v.p2 + e1), e2Int = planeToObjectCoord(v.p2 + e2);

    setColor(color);
    fillConvexPolygon({p2, e2Int, e1Int});
}







Graph::Graph(Widget* parent, int nGraphs, std::vector<Vector> colors, double yScale, double cutStepY, 
    IntVec tl, IntVec br) : 
    BasePlane(parent, tl, br, IntVec(tl.x * 0.8 + br.x * 0.2, tl.y * 0.2 + br.y * 0.8, 0), 
    1, 10, yScale, cutStepY)
{
    this->nGraphs = nGraphs;
    this->nPoints = stdNPoints;
    this->colors = colors;
    this->xScale = width * 1.0 / (nPoints + 1);

    this->points = std::vector<std::vector<double>>(nGraphs, std::vector<double>(nPoints, 0));
}

void Graph::drawGraphs()
{
    for (int nGraph = 0; nGraph < nGraphs; ++nGraph)
    {
        setColor(colors[nGraph]);
        for (int i = 0; i < nPoints; ++i)
        {
            IntVec objectPoint = planeToObjectCoord(Vector(i, -points[nGraph][i], 0));
            if (inRect(objectPoint))
            {
                setColor(colors[nGraph]);
                drawPoint(objectPoint);
            }
        }
    }
}

void Graph::paint()
{
    BasePlane::paint();

    setColor({255, 255, 255});
    drawLine({tl.x, centre.y}, {br.x, centre.y});
    drawLine({centre.x, tl.y}, {centre.x, br.y});

    drawCuts();
    drawGraphs();
}

void Graph::addPoint(std::vector<double> point)
{
    for (int nGraph = 0; nGraph < nGraphs; ++nGraph)
    {
        points[nGraph].push_back(point[nGraph]);
        points[nGraph].erase(points[nGraph].begin());
    }
}





Clock::Clock(Widget* parent, IntVec tl, IntVec br, double scale, double angleStep, double arrowLen) :
    Plane(parent, tl, br, (tl + br) / 2, scale, 1, scale, 1), arrowLen(arrowLen), angleStep(angleStep)
{
    setAxisVisible(0);
    setBorderVisible(0);
    
    nCuts = 12;
    vectors.push_back({{0, 0, 0}, {arrowLen, 0, 0}});
    vectorColors.push_back({255, 255, 255});
}

void Clock::paint()
{
    Plane::paint();

    int pixelArrowLen = arrowLen * xScale;
    setColor({255, 255, 255});
    drawCircle(centre, pixelArrowLen, 0);

    setColor({255, 255, 255});
    for (double angle = 0; angle < 3.1415 * 2; angle += 2 * 3.1415 / nCuts)
    {
        IntVec v1 = centre + IntVec((pixelArrowLen - cutsLength) * std::cos(angle), 
            (pixelArrowLen - cutsLength) * std::sin(angle));
        IntVec v2 = centre + IntVec((pixelArrowLen + cutsLength) * std::cos(angle), 
            (pixelArrowLen + cutsLength) * std::sin(angle));
            
        drawLine({v1.x, v1.y}, {v2.x, v2.y});
    }
}

bool Clock::onIdle(IdleEvent* e)
{
    vectors[0] = rotateV(vectors[0], angleStep);
    return 0;
}