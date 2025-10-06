#include "scroll.h"

#include <cassert>

ScrollBar::ScrollBar(Widget* parent, IntVec tl, IntVec br) : Widget(tl, br, parent)
{
    assert(height > width * 3);

    this->up_button = new ScrollButton({}, {width, width}, this, 1);
    this->down_button = new ScrollButton({0, height - width}, wh, this, 0);

    this->thumb = new ScrollThumb({0, width}, {width, width * 2}, this);
    thumb->setDraggable({0, width}, {width, height - width});

    frac_pos = 0;
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
    bar->moveThumb(up ? -0.1 : 0.1);
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