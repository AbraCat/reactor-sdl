#ifndef PLANEITEM_H
#define PLANEITEM_H

#include <SDL3/SDL.h>

#include <vector>
#include <iostream>

#include "myvector.h"
#include "widget.h"

class PlaneItem : public Widget
{
public:
    PlaneItem(SDL_Renderer* renderer, int nGraphs, std::vector<Vec> colors, double yScale, double cutStepY, IntVec TL, IntVec BR);

    void drawCuts();
    void drawGraphs();
    virtual void paint() override;

    IntVec planeToObjectCoord(Vec coord);
    Vec objectToPlaneCoord(IntVec coord);

    void addPoint(std::vector<double> point);

private:
    IntVec centre;
    double xScale, yScale, cutStepX, cutStepY;

    int nGraphs, nPoints;
    std::vector<std::vector<double>> points;
    std::vector<Vec> colors;
};

#endif // PLANEITEM_H