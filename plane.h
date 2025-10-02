#ifndef PLANEITEM_H
#define PLANEITEM_H

#include <SDL3/SDL.h>

#include <vector>
#include <iostream>

#include "myvector.h"
#include "widget.h"

class BasePlane : public Widget
{
public:
    BasePlane(SDL_Renderer* renderer, IntVec TL, IntVec BR, IntVec centre,
        double xScale, double cutStepX, double yScale, double cutStepY);

    void setAxisVisible(bool axisVisible);
    void drawCuts();
    virtual void paint() override;

    IntVec planeToObjectCoord(Vector coord);
    Vector objectToPlaneCoord(IntVec coord);

protected:
    bool axisVisible;
    IntVec centre;
    double xScale, yScale, cutStepX, cutStepY;
};

class Plane : public BasePlane
{
public:
    Plane(SDL_Renderer* renderer, IntVec TL, IntVec BR, IntVec centre,
        double xScale, double cutStepX, double yScale, double cutStepY);

    void drawVector(FixedVector v, Vector color);
    virtual void paint() override;

    void addPoint(Vector point, Vector color);
    void addVector(FixedVector vec, Vector color);

protected:
    std::vector<FixedVector> vectors;
    std::vector<Vector> points, pointColors, vectorColors;
};

class Graph : public BasePlane
{
public:
    Graph(SDL_Renderer* renderer, int nGraphs, std::vector<Vector> colors, double yScale, double cutStepY, 
        IntVec TL, IntVec BR);

    void drawGraphs();
    virtual void paint() override;

    void addPoint(std::vector<double> point);

private:
    int nGraphs, nPoints;
    std::vector<std::vector<double>> points;
    std::vector<Vector> colors;
};

class Clock : public Plane
{
public:
    Clock(SDL_Renderer* renderer, IntVec TL, IntVec BR, double scale, double angleStep, double arrowLen);
    virtual void paint() override;
    virtual bool onIdle(IdleEvent* e) override;
    
private:
    int nCuts;
    double arrowLen, angleStep;
    FixedVector* arrow;
};

#endif // PLANEITEM_H