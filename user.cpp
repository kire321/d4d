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

    // To use for comparison below
    unsigned new_event_time = new_event->day * 10000 + new_event->hour * 100 +
        new_event->minute;

    // TODO: maybe use a LL to make insertion O(1)
    // Keep in sorted order
    vector<Event*>::iterator ev;
    for (ev = events.begin(); ev != events.end() && (new_event->hour * 60 +
         new_event->minute < (*ev)->hour * 60 + (*ev)->minute); ev++);
    events.insert(ev, new_event);
    cout << to_json(new_event, false) << endl;

    // See if we had a prediction for it; either way, delete all predictions
    // preceeding the event
    vector<Event>::iterator pr;
    unsigned prediction_time;
    for (pr = predictions.begin(); pr != predictions.end(); pr++) {
        prediction_time = pr->day * 10000 + pr->hour * 100 +
            pr->minute;
        if (prediction_time == new_event_time) {
            cout << to_json(&(*pr), true) << endl;
        } else if (prediction_time > new_event_time) {
            break;
        }
    }
    predictions.erase(predictions.begin(), pr);
}

Event* User::get_last_event()
{
    return events.back();
}


void User::next_likely_location(unsigned after_time, unsigned *out_time,
    AntennaId *out_antenna)
{
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
    // FIXME: what if no events??
    if (next_event == NULL) next_event = events.at(0);

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

    *out_time = (event->hour + (event->minute + 30) / 60) % 24;
    *out_antenna = AntennaModel::find_nearest_antenna(smoothed_lat,
        smoothed_lon)->get_id();
}

void User::make_prediction(Path& predicted_path, unsigned day, unsigned hour)
{
    AntennaId predicted_antenna = predicted_path.get_next_step(true);
    while (predicted_antenna >= 0) { // Is a valid antenna
        Event new_predicted_event;
        new_predicted_event.user_id = id;
        new_predicted_event.antenna_id = predicted_antenna;
        if (hour >= 24) {
            hour = 0;
            day++; // Should only happen once, if at all
        }
        new_predicted_event.hour = hour++;
        new_predicted_event.day = day;
        predictions.push_back(new_predicted_event);

        predicted_antenna = predicted_path.get_next_step();
    }
}
