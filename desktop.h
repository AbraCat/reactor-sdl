#ifndef MY_APP_H
#define MY_APP_H

#include "widget.h"
#include "plane.h"
#include "reactor.h"
#include "button.h"
// #include "optical-control.h"
#include "optical-scene.h"

#include "widget.hpp"
#include "container.hpp"
#include "vec2.hpp"

class Desktop : public hui::MyContainerWidget
{
public:
    Desktop(hui::State* state, dr4::Vec2f size);
    ~Desktop();

    virtual void Redraw() override;

    // virtual void updateTexture() override;
    // virtual bool onIdle(IdleEvent* e) override;

private:
    Reactor *reactor;
    Graph *energy_graph, *cnt_graph;
    Clock* clock;
    // WContainer *button_cont;

    // OptController* control;
};

#endif // MY_APP_H