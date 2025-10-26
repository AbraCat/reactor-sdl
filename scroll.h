#ifndef SCROLL_BAR_H
#define SCROLL_BAR_H

#include "wcontainer.h"
#include "button.h"

class ScrollThumb;
class ScrollButton;

class ScrollBar : public Widget
{
public:
    ScrollBar(Widget* parent, Vector tl, Vector br, double init_frac = 0.5);

    double posToFrac(Vector thumbTL);
    Vector fracToPos(double frac);

    void moveThumb(double frac_change);
    void thumbMoved(Vector newThumbTL);
    virtual void action(double frac_pos);

private:
    double frac_pos;
    ScrollButton *up_button, *down_button;
    ScrollThumb *thumb;
};

class ScrollButton : public Button
{
public:
    ScrollButton(Vector tl, Vector br, ScrollBar* bar, bool up, Vector color = {127, 127, 127});
    virtual void action() override;

private:
    bool up;
    ScrollBar *bar;
};

class ScrollThumb : public Button
{
public:
    ScrollThumb(Vector tl, Vector br, ScrollBar* bar, Vector color = {63, 63, 63});
    virtual void action() override;
    virtual void movePos(Vector newTL) override;

private:
    ScrollBar *bar;
};



class MoveScrollBar : public ScrollBar
{
public:
    MoveScrollBar(Widget* parent, Vector tl, Vector br, Widget* w, double amplitude, bool x_axis);

    virtual void action(double frac) override;
    int fracToMovement(double frac);

private:
    Widget *w;
    bool x_axis;
    double init_scale_x, amplitude;
    Vector init_centre;
};

class ScaleScrollBar : public ScrollBar
{
public:
    ScaleScrollBar(Widget* parent, Vector tl, Vector br, Widget* w, double scale_amplitude);
    virtual void action(double frac) override;

    double fracToScale(double frac, bool x);
    double scaleToFrac(double scale, bool x);

private:
    Widget *w;
    double init_scale_x, init_scale_y, scale_amplitude;
};

class ListScrollBar : public ScrollBar
{
public:
    ListScrollBar(Widget* parent, Vector tl, Vector br, WList* list);
    virtual void action(double frac) override;

// private:
    WList* list;
};

#endif // SCROLL_BAR_H