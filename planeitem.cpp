#include "planeitem.h"

const int stdNPoints = 100;
const int axisWidth = 3, cutsWidth = 1, borderWidth = 1, cutsLength = 5, pointSize = 3;

PlaneItem::PlaneItem(SDL_Renderer* renderer, int nGraphs, std::vector<Vector> colors, double yScale, double cutStepY, 
    IntVec TL, IntVec BR) : Widget(renderer, TL, BR)
{
    this->centre = IntVec(TL.x * 0.8 + BR.x * 0.2, TL.y * 0.2 + BR.y * 0.8, 0);

    this->nGraphs = nGraphs;
    this->nPoints = stdNPoints;
    this->colors = colors;
    this->points = std::vector<std::vector<double>>(nGraphs, std::vector<double>(nPoints, 0));

    this->xScale = width * 1.0 / (nPoints + 1);
    this->yScale = yScale;
    this->cutStepX = 10;
    this->cutStepY = cutStepY;
}

void PlaneItem::drawCuts()
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

void PlaneItem::drawGraphs()
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

void PlaneItem::paint()
{
    drawRect(1);
    drawRect(0, Vector(255, 255, 255));

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderLine(renderer, TL.x, centre.y, BR.x, centre.y);
    SDL_RenderLine(renderer, centre.x, TL.y, centre.x, BR.y);

    drawCuts();
    drawGraphs();
}

IntVec PlaneItem::planeToObjectCoord(Vector coord)
{
    IntVec objectCoord;
    objectCoord.x = coord.x * xScale + centre.x;
    objectCoord.y = coord.y * yScale + centre.y;
    return objectCoord;
}

Vector PlaneItem::objectToPlaneCoord(IntVec coord)
{
    Vector planeCoord;
    planeCoord.x = (coord.x - centre.x) * 1.0 / xScale;
    planeCoord.y = (coord.y - centre.y) * 1.0 / yScale;
    return planeCoord;
}

void PlaneItem::addPoint(std::vector<double> point)
{
    for (int nGraph = 0; nGraph < nGraphs; ++nGraph)
    {
        points[nGraph].push_back(point[nGraph]);
        points[nGraph].erase(points[nGraph].begin());
    }
}