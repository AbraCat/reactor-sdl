#include "scroll.h"

#include <cassert>

ScrollBar::ScrollBar(IntVec tl, IntVec br, Widget* parent) : Widget(tl, br, parent)
{
    assert(height > width * 3);

    this->up_button = new ScrollButton(tl, {br.x, tl.y + width}, this, 1);
    this->down_button = new ScrollButton({tl.x, br.y - width}, br, this, 0);
    this->thumb = new ScrollThumb({tl.x, tl.y + width}, {br.x, tl.y + width * 2}, this);

    addChild(up_button);
    addChild(down_button);
    addChild(thumb);

    frac_pos = 0;
    setBorderVisible(1);
}

void ScrollBar::moveThumb(double frac)
{
    frac_pos += frac;
    if (frac_pos < 0) frac_pos = 0;
    if (frac_pos > 1) frac_pos = 1;

    thumb->movePos(fracToPos(frac_pos));
}

double ScrollBar::posToFrac(IntVec thumbTL)
{
    int range = height - width * 3;
    return (thumbTL.y - (tl.y + width)) * 1.0 / range;
}

IntVec ScrollBar::fracToPos(double frac)
{
    int range = height - width * 3;
    return {tl.x, frac * range + tl.y + width};
}



ScrollButton::ScrollButton(IntVec tl, IntVec br, ScrollBar* bar, bool up, Vector color) : 
    Button(tl, br, color, ""), up(up), bar(bar)
{
    //
}

void ScrollButton::action()
{
    bar->moveThumb(up ? 0.1 : -0.1);
}




ScrollThumb::ScrollThumb(IntVec tl, IntVec br, ScrollBar* bar, Vector color) :
    Button(tl, br, color, ""), bar(bar)
{
    //
}

void ScrollThumb::action()
{
    //
}