#ifndef PLANEITEM_H
#define PLANEITEM_H

#include <vector>
#include <iostream>

#include "myvector.h"
#include "widget.h"

class CoordWidget : public Widget
{
public:
    CoordWidget(Widget* parent, IntVec tl, IntVec br, IntVec centre,
        double xScale, double cutStepX, double yScale, double cutStepY);

    void setAxisVisible(bool axisVisible);
    void drawCuts();
    virtual void paint() override;

protected:
    Vector planeTL, planeBR;

    bool axisVisible;
    double cutStepX, cutStepY;
};

class Graph : public CoordWidget
{
public:
    Graph(Widget* parent, int nGraphs, std::vector<Vector> colors, double yScale, double cutStepY, 
        IntVec tl, IntVec br);

    virtual void paint() override;
    void addPoint(std::vector<double> point);

private:
    int nGraphs, nPoints;
    std::vector<std::vector<double>> points;
    std::vector<Vector> colors;
};

class Clock : public CoordWidget
{
public:
    Clock(Widget* parent, IntVec tl, IntVec br, double scale, double angleStep, double arrowLen);
    virtual void paint() override;
    virtual bool onIdle(IdleEvent* e) override;
    
private:
    int nCuts;
    double arrowLen, angleStep;
    FixedVec arrow;
};

#endif // PLANEITEM_H