#include "plane.h"
#include "sdl-adapter.h"

#include <cmath>

const int stdNPoints = 100, planeRectSize = 400;
const int axisWidth = 3, cutsWidth = 1, borderWidth = 1, cutsLength = 5, pointSize = 3;



CoordWidget::CoordWidget(Widget* parent, Vector tl, Vector br, Vector centre,
    double xScale, double cutStepX, double yScale, double cutStepY) : 
    Widget(tl, br, parent), cutStepX(cutStepX), cutStepY(cutStepY)
{
    t->transform(centre, xScale, yScale);

    planeTL = Vector(-planeRectSize / xScale, -planeRectSize / yScale);
    planeBR = Vector(planeRectSize / xScale, planeRectSize / yScale);

    setWidgetBorderVisible(1);
    setFillRect(0);
    axisVisible = 1;
}

void CoordWidget::setAxisVisible(bool axisVisible)
{
    this->axisVisible = axisVisible;
}

void CoordWidget::drawCuts()
{
    for (int x = 0; x < planeBR.x; x += cutStepX)
        t->addLine({{x, -cutsLength / t->yScale}, {x, cutsLength / t->yScale}}, {255, 255, 255});
    for (int x = 0; x > planeTL.x; x -= cutStepX)
        t->addLine({{x, -cutsLength / t->yScale}, {x, cutsLength / t->yScale}}, {255, 255, 255});
    for (int y = 0; y < planeBR.y; y += cutStepY)
        t->addLine({{-cutsLength / t->xScale, y}, {cutsLength / t->xScale, y}}, {255, 255, 255});
    for (int y = 0; y > planeTL.y; y -= cutStepY)
        t->addLine({{-cutsLength / t->xScale, y}, {cutsLength / t->xScale, y}}, {255, 255, 255});
}

void CoordWidget::paint()
{
    Widget::paint();

    if (axisVisible)
    {
        t->addLine({{planeTL.x, 0}, {planeBR.x, 0}}, whiteV);
        t->addLine({{0, planeTL.y}, {0, planeBR.y}}, whiteV);

        drawCuts();
    }
}







Graph::Graph(Widget* parent, int nGraphs, std::vector<Vector> colors, double yScale, double cutStepY, 
    Vector tl, Vector br) : nPoints(stdNPoints),
    CoordWidget(parent, tl, br, Vector((br.x - tl.x) * 0.2, (br.y - tl.y) * 0.8), 
    (br.x - tl.x) * 1.0 / (stdNPoints + 1), 10, yScale, cutStepY)
{
    this->nGraphs = nGraphs;
    this->nPoints = stdNPoints;
    this->colors = colors;

    this->points = std::vector<std::vector<double>>(nGraphs, std::vector<double>(nPoints, 0));
}

void Graph::paint()
{
    CoordWidget::paint();

    for (int nGraph = 0; nGraph < nGraphs; ++nGraph)
    {
        for (int i = 0; i < nPoints; ++i)
        {
            t->addPoint(Vector(i, -points[nGraph][i]), colors[nGraph]);
        }
    }
}

void Graph::addPoint(std::vector<double> point)
{
    for (int nGraph = 0; nGraph < nGraphs; ++nGraph)
    {
        points[nGraph].push_back(point[nGraph]);
        points[nGraph].erase(points[nGraph].begin());
    }
    
    paint();
}





Clock::Clock(Widget* parent, Vector tl, Vector br, double scale, double angleStep, double arrowLen) :
    CoordWidget(parent, tl, br, (br - tl) / 2, scale, 1, scale, 1), arrowLen(arrowLen), angleStep(angleStep)
{
    setAxisVisible(0);
    setTextureBorderVisible(0);
    
    nCuts = 12;
    arrow =  {{0, 0, 0}, {arrowLen, 0, 0}};
}

void Clock::paint()
{
    CoordWidget::paint();

    t->addCircle({}, whiteV, arrowLen, 0);
    
    for (double angle = 0; angle < 3.1415 * 2; angle += 2 * 3.1415 / nCuts)
    {
        Vector v1 = Vector((arrowLen - cutsLength / t->xScale) * std::cos(angle), 
            (arrowLen - cutsLength / t->xScale) * std::sin(angle));
        Vector v2 = Vector((arrowLen + cutsLength / t->xScale) * std::cos(angle), 
            (arrowLen + cutsLength / t->xScale) * std::sin(angle));
            
        t->addLine({v1, v2}, whiteV);
    }

    t->addVector(arrow, whiteV);
}

bool Clock::onIdle(IdleEvent* e)
{
    arrow = rotateV(arrow, angleStep);
    paint();
    return 0;
}
