#include <cstdlib>
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "user.h"
#include "antenna_model.h"
#include "utils.h"

using std::cerr;
using std::endl;
using std::abs;

User::~User()
{
    for (unsigned i = 0; i < events.size(); i++) {
        free(events.at(i));
    }
}

void User::add_event(Event* event)
{
    assert(event->user_id == get_id());

    Event* new_event = (Event*)malloc(sizeof(Event));
    new_event->user_id = get_id();
    new_event->antenna_id = event->antenna_id;
    new_event->time = event->time;

    // Keep in sorted order
    vector<Event*>::iterator pos = std::upper_bound(events.begin(),
        events.end(), new_event, earlier_event_time);
    events.insert(pos, new_event);

    Event* last_event = get_last_event();
    // TODO: this will also change
    int num_steps = estimate_num_steps(last_event->time, new_event->time);
    Path interpolated_path = Path::interpolate_path(last_event->antenna_id,
        new_event->antenna_id, num_steps);
    AntennaId current_antenna, next_antenna;
    current_antenna = interpolated_path.get_next_step(true);
    while ((next_antenna = interpolated_path.get_next_step()) != -1) {
        // We only care about bona fide transitions
        if (current_antenna != next_antenna) {
            transitions[current_antenna].push_back(next_antenna);
            current_antenna = next_antenna;
        }
    }

    last_event = new_event;
}

AntennaId User::make_prediction(ptime time)
{
    Event previous;
    previous_event(&previous);
    if (previous.antenna_id < 0) {
        return -1;
    } else {
      // FIXME: the bread and butter of algorithm
        vector<AntennaId> next_steps = transitions[previous.antenna_id];
        if (next_steps.size() != 0) {
            return next_steps.at(rand() % next_steps.size());
        } else {
            return -1;
        }
    }
}

int User::estimate_num_steps(ptime start, ptime end)
{
  // FIXME: tune this
    return (to_minutes(start - end) / step_size_at(start.time_of_day())) + 0.5;
}

void User::previous_event(Event* previous)
{
    if (last_event) {
        *previous= *last_event;
    } else { // Error; only occurs the first time
        previous->antenna_id = -1;
    }
}

int User::step_size_at(time_duration time)
{
  // FIXME: tune this
    return 15;
}
