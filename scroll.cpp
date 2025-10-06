#include "scroll.h"

#include <cassert>

ScrollBar::ScrollBar(Widget* parent, IntVec tl, IntVec br) : Widget(tl, br, parent)
{
    assert(height > width * 3);

    this->up_button = new ScrollButton({}, {width, width}, this, 1);
    this->down_button = new ScrollButton({0, height - width}, wh, this, 0);

    this->thumb = new ScrollThumb({0, width}, {width, width * 2}, this);
    thumb->setDraggable({0, width}, {width, height - width});
    moveThumb(0.5);

    frac_pos = 0.5;
    setTextureBorderVisible(1);
}

void ScrollBar::moveThumb(double frac)
{
    // printf("%lf %d %d\n", frac, thumb->tl.x, thumb->tl.y);
    frac_pos += frac;
    if (frac_pos < 0) frac_pos = 0;
    if (frac_pos > 1) frac_pos = 1;

    thumb->movePos(fracToPos(frac_pos));
}

double ScrollBar::posToFrac(IntVec thumbTL)
{
    int range = height - width * 3;
    return (thumbTL.y - width) * 1.0 / range;
}

IntVec ScrollBar::fracToPos(double frac)
{
    int range = height - width * 3;
    return {0, frac * range + width};
}

void ScrollBar::thumbMoved(IntVec newThumbTL)
{
    frac_pos = posToFrac(newThumbTL);
    action(frac_pos);
}

void ScrollBar::action(double frac)
{
    // printf("action %lf\n", frac);
}



ScrollButton::ScrollButton(IntVec tl, IntVec br, ScrollBar* bar, bool up, Vector color) : 
    Button(bar, tl, br, color, ""), up(up), bar(bar)
{
    //
}

void ScrollButton::action()
{
    bar->moveThumb(up ? -0.05 : 0.05);
}




ScrollThumb::ScrollThumb(IntVec tl, IntVec br, ScrollBar* bar, Vector color) :
    Button(bar, tl, br, color, ""), bar(bar)
{
    //
}

void ScrollThumb::movePos(IntVec newTL)
{
    Widget::movePos(newTL);
    bar->thumbMoved(newTL);
}

void ScrollThumb::action()
{
    //
}



MoveScrollBar::MoveScrollBar(Widget* parent, IntVec tl, IntVec br, Widget* w, IntVec movement) :
    ScrollBar(parent, tl, br), w(w)
{
    minPos = w->t->centre - movement;
    maxPos = w->t->centre + movement;
}

void MoveScrollBar::action(double frac)
{
    w->t->move(minPos * (1 - frac) + maxPos * frac);
}

ScaleScrollBar::ScaleScrollBar(Widget* parent, IntVec tl, IntVec br, Widget* w, double change) :
    ScrollBar(parent, tl, br), w(w)
{
    min_scale = 1 / change;
    max_scale = change;
}

void ScaleScrollBar::action(double frac)
{
    w->t->rescaleCentre(min_scale * (1 - frac) + max_scale * frac); // change formula
}