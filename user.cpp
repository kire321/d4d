#include <cstdlib> // TODO: remove me
#include <assert.h>
#include <iostream>
#include <sstream>

#include "user.h"
#include "antenna_model.h"

using std::cout;
using std::endl;
using std::stringstream;

void User::to_event(valarray<int> event_data, Event* event)
{
    event->user_id = event_data[EV_UID];
    event->antenna_id = event_data[EV_ANTENNA];
    event->day = (event_data[EV_YEAR] - 2000) * 10000 +
        event_data[EV_MONTH] * 100 + event_data[EV_DAY]; // Hack to get unique days
    event->hour = event_data[EV_HOUR];
    event->minute = event_data[EV_MINUTE];
}

string User::to_json(Event* event, bool is_prediction)
{
    stringstream json;
    json << "{" << endl;
    json << "\t\"uid\" : " << event->user_id << "," << endl;
    json << "\t\"time\" :" << endl;
    json << "\t{" << endl;
    json << "\t\t\"day\" : " << event->day << "," << endl;
    json << "\t\t\"hour\" : " << event->hour << endl;
    json << "\t}," << endl;
    json << "\t\"antenna\" : " << event->antenna_id << "," << endl;
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
    new_event->day = event->day;
    new_event->hour = event->hour;
    new_event->minute = event->minute;

    // TODO: maybe use a LL to make insertion O(1)
    // Keep in sorted order
    vector<Event*>::iterator ev;
    for (ev = events.begin(); ev != events.end() && (new_event->hour * 60 +
         new_event->minute < (*ev)->hour * 60 + (*ev)->minute); ev++);
    events.insert(ev, new_event);

    last_event = new_event;
}

void User::next_likely_event(Event* after_event, Event* likely_event)
{
    if (events.empty()) {
        likely_event->antenna_id = -1;
        return;
    }

    unsigned after_time = after_event-> hour;
    unsigned after_minute = (after_time + 1) * 60;

    Event* next_event = NULL;
    Event* event = NULL;
    for (unsigned i = 0; i < events.size(); i++) {
        event = events.at(i);
        unsigned event_minute = event->hour * 60 + event->minute;
        if (event_minute > after_minute) {
            next_event = event;
        }
    }

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
    smoothed_lat /= weight_sum;
    smoothed_lon /= weight_sum;

    likely_event->user_id = id;
    likely_event->day = after_event->day;
    likely_event->hour = (event->hour * 60 + (event->minute + 30) / 60);
    if (likely_event->hour >= 24) {
        likely_event->hour %= 24;
        likely_event->day += 1;
    }
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
