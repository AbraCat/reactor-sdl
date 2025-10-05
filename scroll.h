#ifndef SCROLL_BAR_H
#define SCROLL_BAR_H

#include "widget.h"
#include "button.h"

class ScrollThumb;
class ScrollButton;

class ScrollBar : public Widget
{
public:
    ScrollBar(IntVec tl, IntVec br, Widget* parent = nullptr);
    void moveThumb(double frac);

    double posToFrac(IntVec thumbTL);
    IntVec fracToPos(double frac);

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

private:
    ScrollBar *bar;
};

#endif // SCROLL_BAR_H