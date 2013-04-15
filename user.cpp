#include "user.h"
#include <cstdlib> // TODO: remove me
#include <assert.h>

multiDimVala<float> User::antennas = multiDimVala<float>();

void User::to_event(valarray<int> event_data, Event* event)
{
    event->user_id = event_data[EV_UID];
    event->antenna_id = event_data[EV_ANTENNA];
    event->time = event_data[EV_HOUR]; // FIXME: no minutes in here
}

User::~User()
{
  // TODO: delete all vevents
}

void User::add_event(Event* event)
{
    assert(event->user_id == get_id());

    Event* new_event = malloc(Event);
    new_event->user_id = get_id();
    new_event->antenna_id = event->antenna_id;
    new_event->time = event->time;

    events->push_back(new_event);

    set_dirty();
}

void User::addEvent(valarray<int> newEvent) {
    //number of seconds is always zero
    add_event(to_event(newEvent));
    times.push_back(newEvent[EV_HOUR]*60+newEvent[EV_MINUTE]);
    original.push_back(antennas.getCopy(0,newEvent[EV_ANTENNA]));
    set_dirty();
}

void User::smooth() {
    smoothedUpToDate=true;
    multiDimVala<float> vala(original);
    smoothed=multiDimVala<float>(original.size(),2);
    for (unsigned i=0; i<original.size(); ++i) {
        multiDimVala<float> gauss(times.size(),2);
        for (unsigned j=0; j<times.size(); ++j)
            gauss.getView(0,j)=pdf(normal(times[i],60),times[j]);
        valarray<float> denom=gauss.sum(0);
        gauss.data*=vala.data;
        smoothed.getView(0,i)=gauss.sum(0)/denom;
    }    
}

multiDimVala<float> User::getSmoothed()
{
    if (is_dirty()) smooth();

    return smoothed;
}

Event* get_last_event()
{
    return events->back();
}

AntennaId User::next_likely_location(unsigned after_time)
{
    // FIXME: need smoothed times to be sorted
    return events->back()->antenna_id;
}
