#include <cstdlib> // TODO: remove me
#include <assert.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

#include <boost/date_time/gregorian/gregorian.hpp>

#include "user.h"
#include "antenna_model.h"
#include "utils.h"

using std::cerr;
using std::endl;
using std::abs
using std::stringstream;
using boost::gregorian::date;
using boost::gregorian::date_duration;

void User::to_event(valarray<int> event_data, Event* event)
{
    event->user_id = event_data[EV_UID];
    event->antenna_id = event_data[EV_ANTENNA];
    event->time = ptime(date(event_data[EV_YEAR], event_data[EV_MONTH],
        event_data[EV_DAY]), time_duration(event_data[EV_HOUR],
        event_data[EV_MINUTE], event_data[EV_SECOND]));
    // Can't use time_from_string because we're passing a valarray of ints
}

string User::to_json(Event* event, bool is_prediction)
{
    stringstream json;
    json << "{" << endl;
    json << "\t\"uid\" : " << event->user_id << "," << endl;
    json << "\t\"antenna\" : " << event->antenna_id << "," << endl;
    json << "\t\"time\" :" << to_simple_string(event->time) << endl;
    json << "\t\"is_prediction\" : " << (is_prediction ? "true" : "false")
        << endl;
    json << "}";

    return json.str();
}

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

    last_event = new_event;
}

AntennaId User::smooth(time_duration time) {
    unsigned next_event_minute = to_minutes(time.time_of_day());

    float smoothed_lat = 0;
    float smoothed_lon = 0;
    float weight_sum = 0;
    for(unsigned i = 0; i < events.size(); i++) {
        event = events.at(i);
        unsigned event_minute = to_minutes(event.time.time_of_day());
	Antenna* antenna =
		AntennaModel::find_antenna_by_id(event->antenna_id);

	int dif = abs(event_minute - next_event_minute);
	if(dif > 12 * 60)
		dif = 24 * 60 - dif;
	float weight = pdf(normal(0, 30), dif);
        weight_sum += weight;
        smoothed_lat += weight * antenna->get_latitude();
        smoothed_lon += weight * antenna->get_longitude();
    }
    smoothed_lat /= weight_sum;
    smoothed_lon /= weight_sum;

    return AntennaModel::find_nearest_antenna(smoothed_lat,
        smoothed_lon)->get_id();
}

void User::next_likely_event(Event* after_event, Event* likely_event)
{
    if (events.empty()) {
        likely_event->antenna_id = -1;
        return;
    }

    // Set User ID
    likely_event->user_id = after_event->user_id;

    // Set time
    vector<Event*>::iterator ev = std::upper_bound(events.begin(), events.end(),
        after_event, earlier_event_time);
    bool add_day = false;
    if (ev == events.end()) {
        ev = events.begin();
        add_day = true;
    }
    ptime next_time(after_event->time.date(), (*ev)->time.time_of_day());
    // TODO: clean this up
    if (to_minutes(next_time - after_event->time) < AntennaModel::timestep) {
        if (++ev == events.end()) {
            ev = events.begin();
            add_day = true;
        }
        next_time = ptime(after_event->time.date(), (*ev)->time.time_of_day());
    }
    likely_event->time = next_time;
    if (add_day) likely_event->time += date_duration(1);


    // Set location = smoothed antenna
    /////////////////////
    // FIXME
    unsigned next_event_minute = to_minutes((*ev)->time.time_of_day());

    float smoothed_lat = 0;
    float smoothed_lon = 0;
    float weight_sum = 0;
    unsigned next_event_minute = next_event->hour * 60 + next_event->minute;
    for (unsigned i = 0; i < events.size(); i++) {
        event = events.at(i);
        unsigned event_minute = event->hour * 60 + event->minute;
        Antenna* antenna = AntennaModel::find_antenna_by_id(event->antenna_id);

        float weight = pdf(normal(next_event_minute, 30), event_minute);
        weight_sum += weight;
        smoothed_lat += weight * antenna->get_latitude();
        smoothed_lon += weight * antenna->get_longitude();
    }
    // END FIXME
    /////////////////////
    smoothed_lat /= weight_sum;
    smoothed_lon /= weight_sum;

    likely_event->antenna_id = AntennaModel::find_nearest_antenna(smoothed_lat,
        smoothed_lon)->get_id();
}

void User::previous_event(Event* previous_event)
{
    if (last_event) {
        *previous_event = *last_event;
    } else { // Error; only occurs the first time
        previous_event->antenna_id = -1;
    }
}
