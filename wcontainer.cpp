#include "wcontainer.h"

#include "container.hpp"



hui::ContainerWidget::ContainerWidget(hui::State *state, dr4::Vec2f pos, dr4::Vec2f size)
    : hui::Widget(state, pos, size)
{
    //
}

hui::MyContainerWidget::MyContainerWidget(hui::State *state, dr4::Vec2f pos, dr4::Vec2f size)
    : hui::ContainerWidget(state, pos, size)
{
    //
}

hui::MyContainerWidget::~MyContainerWidget()
{
    for (hui::Widget* w: children)
    {
        delete w;
    }
}

void hui::MyContainerWidget::DrawOnParent()
{
    for (hui::Widget* w: children)
    {
        w->DrawOnParent();
    }

    hui::Widget::DrawOnParent();
}

void hui::MyContainerWidget::addChild(hui::Widget* w)
{
    children.push_back(w);
    w->SetParent(this);
}

hui::EventResult hui::MyContainerWidget::PropogateToChildren(Event &event)
{
    // TODO: change event coordinates

    for (Widget* w: children)
    {
        EventResult res = hui::EventResult::UNHANDLED;
        ContainerWidget* container = dynamic_cast<ContainerWidget*>(w);

        if (container == nullptr) res = event.Apply(*w);
        else res = container->PropogateToChildren(event);

        if (res == EventResult::HANDLED) return res;
    }

    return event.Apply(*this);
}





WContainer::WContainer(Widget* parent, Vector tl, Vector br, int nChildren, bool vertical)
    : Widget(tl, br, parent)
{
    setWidgetBorderVisible(1);
    this->vertical = vertical;
    this->nChildren = nChildren;
    this->padding = 0;

    if (vertical)
    {
        this->childHeight = (height - padding * (nChildren + 1)) / nChildren;
        this->childWidth = width - 2 * padding;
    }
    else
    {
        this->childWidth = (width - padding * (nChildren + 1)) / nChildren;
        this->childHeight = height - 2 * padding;
    }
}

void WContainer::resizeChild(int nChild)
{
    Widget* w = children[nChild];

    if (vertical) w->resize(Vector(padding, padding * (nChild + 1) + childHeight * nChild), 
        Vector(wh.x - padding, (padding + childHeight) * (nChild + 1)));

    else w->resize(Vector(padding * (nChild + 1) + childWidth * nChild, padding), 
        Vector((padding + childWidth) * (nChild + 1), wh.y - padding));
}

void WContainer::addWidget(Widget* w)
{
    // assert(children.size() < nChildren);
    int nChild = children.size();
    Widget::addWidget(w);

    w->t->setVisibleIn(this);
    resizeChild(nChild);    
}




WList::WList(Widget* parent, Vector tl, Vector br, bool vertical, double child_len)
    : WContainer(parent, tl, br, 1, vertical), child_len(child_len)
{
    setFillRect(1);
    scroll_frac = 0;

    if (vertical)
    {
        childWidth = width;
        childHeight = child_len;
    }
    else
    {
        childHeight = height;
        childWidth = child_len;
    }
}

int WList::removeChildByPredicate(std::function<bool(Widget*)> predicate)
{
    int n_removed = Widget::removeChildByPredicate(predicate);

    for (int n_child = n_removed; n_child < children.size(); ++n_child)
        resizeChild(n_child);

    updateTextureRec();
    return n_removed;
}

Vector WList::propagatedAbsTL()
{
    if (vertical)
        return Vector(absTL.x, absTL.y - scroll_frac * (child_len * children.size() - height));
    else
        return Vector(absTL.x - scroll_frac * (child_len * children.size() - width), absTL.y);
}

void WList::scroll(double frac)
{
    scroll_frac = frac;
    propagateAbsPos();
    t->updated = 1;
}

bool WList::handleEvent(Event* e)
{
    MouseEvent* press_evt = dynamic_cast<MouseEvent*>(e);
    if (press_evt != NULL && press_evt->type == MOUSE_DOWN &&
        !inAbsRect(Vector(press_evt->x, press_evt->y)))
    {
        return e->dispatch(this);
    }

    for (Widget* w: children)
        if (w->handleEvent(e)) return 1;
    return e->dispatch(this);
}

