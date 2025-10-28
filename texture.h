#ifndef MY_TEXTURE_H
#define MY_TEXTURE_H

#include "myvector.h"

#include <vector>
#include <string>

class Widget;


class CoordSystem
{
public:
    CoordSystem(Vector centre, double xScale, double yScale);

    Vector tranformToBaseCoord(Vector coord);
    Vector baseToTransformCoord(Vector coord);

    virtual void transform(Vector centre, double xScale, double yScale);
    void move(Vector newPos);
    void rescale(double new_scale_x, double new_scale_y);
    void rescale(double new_scale_x, double new_scale_y, Vector point);

    virtual Vector getAbsCentre();

// protected:
    Vector centre;
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
    virtual void render();
    void renderRec();
    // void renderIfUpdated();
    // void renderIfUpdatedRec();
    void clear();

    void setVisibleIn(Widget* w);

    virtual Vector getAbsCentre() override;
    void rescaleCentre(double new_scale_x, double new_scale_y);

    void addText(std::string text);
    virtual void addPoint(Vector p, Vector color);
    void addLine(FixedVec line, Vector color);

    void addRect(FixedVec rect, Vector color, bool fill);
    void addCircle(Vector centre, Vector col, double r, bool fill);
    void addPolygon(std::vector<Vector> points, Vector color, bool fill);
    void addVector(FixedVec v, Vector color);

    void paintPoint(ColPoint p);
    void paintLine(ColFixedVec line);
    void paintRect(ColFixedVec rect);
    void paintCircle(ColCircle cirlce);
    void paintPolygon(ColPolygon polygon);
    void paintText();

// protected:
    bool updated;
    Widget *w, *visible_in;

    std::string text;
    std::vector<ColPoint> points;
    std::vector<ColFixedVec> lines, rects;
    std::vector<ColCircle> circles;
    std::vector<ColPolygon> polygons;
};

class PixelTexture : public Texture
{
public:
    PixelTexture(Widget* w);
    ~PixelTexture();
    virtual void render() override;

    void setPix(int x, int y, Vector col);
    Vector getPix(int x, int y);

    Vector* pix;
};

#endif // MY_TEXTURE_H