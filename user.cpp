#include <cstdlib> // TODO: remove me
#include <assert.h>
#include <iostream>
#include <sstream>

// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/json_parser.hpp>
// 
// using boost::property_tree::ptree;
// using boost::property_tree::write_json;

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

    events.push_back(new_event);
    cout << to_json(new_event, false) << endl;

    // See if we had a prediction for it; either way, delete all predictions
    // preceeding the event
    vector<Event*>::iterator pr = predictions.begin();
    while (pr != predictions.end()) {
        unsigned prediction_time = (*pr)->day * 10000 + (*pr)->hour * 100 +
            (*pr)->minute;
        if (prediction_time == new_event_time) {
            cout << to_json((*pr), true) << endl;
        } else if (prediction_time > new_event_time) {
            break;
        }
        pr++;
    }
    predictions.erase(predictions.begin(), pr);

    set_dirty();
}

void User::smooth() {
    // TODO: clean up this hack of recreating erik's data
    vector<int> times;
    vector<valarray<float> > original;
    vector<Event*>::iterator event;
    for (event = events.begin(); event != events.end(); event++) {
        times.push_back((*event)->hour * 60 + (*event)->minute);
        Antenna* antenna =
            AntennaModel::find_antenna_by_id((*event)->antenna_id);
        valarray<float> temp(2);
        temp[0] = antenna->get_latitude();
        temp[1] = antenna->get_longitude();
        original.push_back(temp);
    }

    num_unsmoothed_events = 0;

    multiDimVala<float> vala(original);
    smoothed = multiDimVala<float>(original.size(),2);
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

Event* User::get_last_event()
{
    return events.back();
}

typedef pair<int,int> ii;
bool comparison(ii i, ii j) {
	//sort based on the first value
	return i.first<j.first;
}


// TODO: out_time is length or #?
void User::next_likely_location(unsigned after_time, unsigned *out_time, AntennaId *out_ant)
{
    vector<ii> zipped;
    for (unsigned i = 0; i < events.size(); ++i) {
        int event_minute = events[i]->hour * 60 + events[i]->minute;
        zipped.push_back(ii(event_minute,i));
    }
    sort(zipped.begin(), zipped.end(), comparison);
    //The following slow search doesn't affect running time since we just did a sort which is slower
    unsigned i = 0;
    for (; i < zipped.size(); i++) {
        if (zipped[i].first > (int)(after_time+1)*60) break;
    }
    if (i == zipped.size()) i = 0;
    ii pair1 = zipped[i];
    *out_time = (pair1.first + 30)/60;
    valarray<float> latLon=getSmoothed().getCopy(0,zipped[i].second);
    *out_ant=AntennaModel::find_nearest_antenna(latLon[0],latLon[1])->get_id();
}

void User::make_prediction(Path& predicted_path, unsigned day, unsigned hour)
{
    AntennaId predicted_antenna = predicted_path.get_next_step(true);
    while (predicted_antenna >= 0) { // Is a valid antenna
        Event* new_predicted_event = (Event*)malloc(sizeof(Event));
        new_predicted_event->user_id = id;
        new_predicted_event->antenna_id = predicted_antenna;
        if (hour >= 24) {
            hour = 0;
            day++; // Should only happen once, if at all
        }
        new_predicted_event->hour = hour++;
        new_predicted_event->day = day;
        predictions.push_back(new_predicted_event);

        predicted_antenna = predicted_path.get_next_step();
    }
}
