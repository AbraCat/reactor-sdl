#include "plane.h"

#include <cmath>

const int stdNPoints = 100;
const int axisWidth = 3, cutsWidth = 1, borderWidth = 1, cutsLength = 5, pointSize = 3;

const double arrowCoeff = 0.1;



BasePlane::BasePlane(SDL_Renderer* renderer, IntVec TL, IntVec BR, IntVec centre,
    double xScale, double cutStepX, double yScale, double cutStepY) : Widget(renderer, TL, BR),
    xScale(xScale), yScale(yScale), cutStepX(cutStepX), cutStepY(cutStepY), centre(centre)
{
    axisVisible = 1;
}

void BasePlane::setAxisVisible(bool axisVisible)
{
    this->axisVisible = axisVisible;
}

void BasePlane::drawCuts()
{
    for (int x = centre.x; x < BR.x; x += xScale * cutStepX)
        SDL_RenderLine(renderer, x, centre.y - cutsLength, x, centre.y + cutsLength);
    for (int x = centre.x; x > TL.x; x -= xScale * cutStepX)
        SDL_RenderLine(renderer, x, centre.y - cutsLength, x, centre.y + cutsLength);
    for (int y = centre.y; y < BR.y; y += yScale * cutStepY)
        SDL_RenderLine(renderer, centre.x - cutsLength, y, centre.x + cutsLength, y);
    for (int y = centre.y; y > TL.y; y -= yScale * cutStepY)
        SDL_RenderLine(renderer, centre.x - cutsLength, y, centre.x + cutsLength, y);
}

void BasePlane::paint()
{
    drawRect(1);
    drawRect(0, Vector(255, 255, 255));

    if (axisVisible)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderLine(renderer, TL.x, centre.y, BR.x, centre.y);
        SDL_RenderLine(renderer, centre.x, TL.y, centre.x, BR.y);

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


Plane::Plane(SDL_Renderer* renderer, IntVec TL, IntVec BR, IntVec centre,
    double xScale, double cutStepX, double yScale, double cutStepY) :
    BasePlane(renderer, TL, BR, centre, xScale, cutStepX, yScale, cutStepY)
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
        SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);
    
        SDL_RenderPoint(renderer, planePoint.x, planePoint.y);
    }

    for (int nVec = 0; nVec < vectors.size(); ++nVec)
    {
        FixedVector vec = vectors[nVec];
        Vector color = vectorColors[nVec];
        SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);
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
    SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);
    IntVec p1 = planeToObjectCoord(v.p1), p2 = planeToObjectCoord(v.p2);
    SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);

    // Vector vec = fixedToFree(v);
    // Vector ort1 = {vec.y, -vec.x}, ort2 = {-vec.y, vec.x};
    // Vector e1 = (ort1 - vec) * arrowCoeff, e2 = (ort2 - vec) * arrowCoeff;
    // IntVec e1Int = planeToObjectCoord(v.p2 + e1), e2Int = planeToObjectCoord(v.p2 + e2);

    // DRAW ARROW

    // QPolygon polygon;
    // polygon << QPoint(p2.x, -p2.y) << QPoint(e1Int.x, -e1Int.y) << QPoint(e2Int.x, -e2Int.y);
    // painter->setBrush(Qt::black);
    // painter->drawPolygon(polygon);
    // painter->setBrush(Qt::transparent);
}







Graph::Graph(SDL_Renderer* renderer, int nGraphs, std::vector<Vector> colors, double yScale, double cutStepY, 
    IntVec TL, IntVec BR) : 
    BasePlane(renderer, TL, BR, IntVec(TL.x * 0.8 + BR.x * 0.2, TL.y * 0.2 + BR.y * 0.8, 0), 
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
        SDL_SetRenderDrawColor(renderer, colors[nGraph].x, colors[nGraph].y, colors[nGraph].z, 255);
        for (int i = 0; i < nPoints; ++i)
        {
            IntVec objectPoint = planeToObjectCoord(Vector(i, -points[nGraph][i], 0));
            if (inRect(objectPoint))
            {
                SDL_SetRenderDrawColor(renderer, colors[nGraph].x, colors[nGraph].y, colors[nGraph].z, 255);
                SDL_RenderPoint(renderer, objectPoint.x, objectPoint.y);
            }
        }
    }
}

void Graph::paint()
{
    drawRect(1);
    drawRect(0, Vector(255, 255, 255));

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderLine(renderer, TL.x, centre.y, BR.x, centre.y);
    SDL_RenderLine(renderer, centre.x, TL.y, centre.x, BR.y);

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





Clock::Clock(SDL_Renderer* renderer, IntVec TL, IntVec BR, double scale, double angleStep, double arrowLen) :
    Plane(renderer, TL, BR, (TL + BR) / 2, scale, 1, scale, 1), arrowLen(arrowLen), angleStep(angleStep)
{
    setAxisVisible(0);

    vectors.push_back({{0, 0, 0}, {arrowLen, 0, 0}});
    vectorColors.push_back({255, 255, 255});
}

void Clock::paint()
{
    Plane::paint();

    //
}

bool Clock::onIdle(IdleEvent* e)
{
    vectors[0] = rotateV(vectors[0], angleStep);
    return 0;
}