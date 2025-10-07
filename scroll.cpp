#include "scroll.h"

#include <cassert>
#include <cmath>

ScrollBar::ScrollBar(Widget* parent, IntVec tl, IntVec br) : Widget(tl, br, parent)
{
    assert(height > width * 3);
    setTextureBorderVisible(1);

    this->up_button = new ScrollButton({}, {width, width}, this, 1);
    this->down_button = new ScrollButton({0, height - width}, wh, this, 0);

    this->thumb = new ScrollThumb({0, width}, {width, width * 2}, this);
    thumb->setDraggable({0, width}, {width, height - width});
    frac_pos = 0;

    moveThumb(0.5);
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

void ScrollBar::moveThumb(double frac)
{
    frac_pos += frac;
    if (frac_pos < 0) frac_pos = 0;
    if (frac_pos > 1) frac_pos = 1;

    thumb->movePos(fracToPos(frac_pos));
}

void ScrollBar::thumbMoved(IntVec newThumbTL)
{
    frac_pos = posToFrac(newThumbTL);
    action(frac_pos);
}

void ScrollBar::action(double frac)
{
    //
}



ScrollButton::ScrollButton(IntVec tl, IntVec br, ScrollBar* bar, bool up, Vector color) : 
    Button(bar, tl, br, color, ""), up(up), bar(bar)
{
    //
}

void ScrollButton::action()
{
    const double move_on_click = 0.05;
    bar->moveThumb(up ? -move_on_click : move_on_click);
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



MoveScrollBar::MoveScrollBar(Widget* parent, IntVec tl, IntVec br, Widget* w, 
    IntVec amplitude, bool x_axis) : ScrollBar(parent, tl, br)
{
    this->w = w;
    this->x_axis = x_axis;
    this->amplitude = amplitude;

    this->init_centre = w->t->centre;
    this->init_scale_x = w->t->xScale;
}

IntVec MoveScrollBar::fracToMovement(double frac)
{
    double scale_change = w->t->xScale / init_scale_x;
    IntVec scaled_init_centre = w->wh * 0.5 + ((Vector)init_centre - w->wh * 0.5) * scale_change;
    
    IntVec min_pos = (Vector)scaled_init_centre - amplitude * scale_change;
    IntVec max_pos = (Vector)scaled_init_centre + amplitude * scale_change;

    return min_pos * (1 - frac) + max_pos * frac;
}

double MoveScrollBar::movementToFrac(IntVec movement)
{
    return 0;
}

void MoveScrollBar::action(double frac)
{
    IntVec movement = fracToMovement(frac);
    if (x_axis) movement.y = w->t->centre.y;
    else movement.x = w->t->centre.x;

    w->t->move(movement);
}

ScaleScrollBar::ScaleScrollBar(Widget* parent, IntVec tl, IntVec br, Widget* w, double scale_amplitude) :
    ScrollBar(parent, tl, br), w(w)
{
    this->scale_amplitude = scale_amplitude;
    init_scale_x = w->t->xScale;
    init_scale_y = w->t->yScale;
}

double ScaleScrollBar::fracToScale(double frac, bool x)
{
    double init_scale = (x ? init_scale_x : init_scale_y);
    double min_scale = init_scale / scale_amplitude;

    return min_scale * std::exp(std::log(scale_amplitude) * frac * 2);
}

double ScaleScrollBar::scaleToFrac(double scale, bool x)
{
    // f(scale / min_scale) = frac
    // f(1) = 0
    // f(scale_amp) = 0.5
    // f (scale_amp ^ 2) = 1
    // f(x) = log_{scale_amp^2}(x) = 0.5 * log_{scale_amp}(x)

    double init_scale = (x ? init_scale_x : init_scale_y);
    double min_scale = init_scale / scale_amplitude;

    return 0.5 * std::log(scale / min_scale) / std::log(scale_amplitude);
}

void ScaleScrollBar::action(double frac)
{
    w->t->rescaleCentre(fracToScale(frac, 1), fracToScale(frac, 0));
}