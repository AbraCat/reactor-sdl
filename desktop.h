#ifndef MY_APP_H
#define MY_APP_H

#include "widget.h"
#include "plane.h"
#include "reactor.h"
#include "button.h"
#include "optical-control.h"

class Desktop : public Widget
{
public:
    Desktop();
    ~Desktop();

    virtual void paint() override;
    virtual bool onIdle(IdleEvent* e) override;

private:
    Reactor *reactor;
    Graph *energy_graph, *cnt_graph;
    Clock* clock;
    WContainer *button_cont;

    OptController* control;
};

#endif // MY_APP_H