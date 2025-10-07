#ifndef SCROLL_BAR_H
#define SCROLL_BAR_H

#include "widget.h"
#include "button.h"

class ScrollThumb;
class ScrollButton;

class ScrollBar : public Widget
{
public:
    ScrollBar(Widget* parent, IntVec tl, IntVec br);

    double posToFrac(IntVec thumbTL);
    IntVec fracToPos(double frac);

    void moveThumb(double frac);
    void thumbMoved(IntVec newThumbTL);
    virtual void action(double frac);

private:
    double frac_pos;
    ScrollButton *up_button, *down_button;
    ScrollThumb *thumb;
};

class ScrollButton : public Button
{
public:
    ScrollButton(IntVec tl, IntVec br, ScrollBar* bar, bool up, Vector color = {127, 127, 127});
    virtual void action() override;

private:
    bool up;
    ScrollBar *bar;
};

class ScrollThumb : public Button
{
public:
    ScrollThumb(IntVec tl, IntVec br, ScrollBar* bar, Vector color = {63, 63, 63});
    virtual void action() override;
    virtual void movePos(IntVec newTL) override;

private:
    ScrollBar *bar;
};



class MoveScrollBar : public ScrollBar
{
public:
    MoveScrollBar(Widget* parent, IntVec tl, IntVec br, Widget* w, IntVec amplitude, bool x_axis);
    virtual void action(double frac) override;

    IntVec fracToMovement(double frac);
    double movementToFrac(IntVec movement);

private:
    Widget *w;
    bool x_axis;
    double init_scale_x;
    IntVec init_centre, amplitude;
};

class ScaleScrollBar : public ScrollBar
{
public:
    ScaleScrollBar(Widget* parent, IntVec tl, IntVec br, Widget* w, double scale_amplitude);
    virtual void action(double frac) override;

    double fracToScale(double frac, bool x);
    double scaleToFrac(double scale, bool x);

private:
    Widget *w;
    double init_scale_x, init_scale_y, scale_amplitude;
};

#endif // SCROLL_BAR_H