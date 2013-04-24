#include <cstdlib> // TODO: remove me
#include <assert.h>
#include <iostream>

// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/json_parser.hpp>
// 
// using boost::property_tree::ptree;
// using boost::property_tree::write_json;

#include "user.h"
#include "antenna_model.h"

multiDimVala<float> User::antennas = multiDimVala<float>();

void User::to_event(valarray<int> event_data, Event* event)
{
    event->user_id = event_data[EV_UID];
    event->antenna_id = event_data[EV_ANTENNA];
    event->day = (event_data[EV_YEAR] - 2000) * 10000 +
        event_data[EV_MONTH] * 100 + EV_DAY; // Hack to get unique days
    event->hour = event_data[EV_HOUR];
}

string User::to_json(Event* event, bool is_prediction)
{
    string json("{\n");
    json.append("\t\"uid\" : %d\n", event->user_id);
    json.append("\t\"time\" :\n");
    json.append("\t{\n");
    json.append("\t\t\"day\" : %d\n", event->day);
    json.append("\t\t\"hour\" : %d\n", event->hour);
    json.append("\t}\n");
    json.append("\t\"antenna\" : %d\n", event->antenna_id);
    json.append("\t\"is_prediction\" : %s\n",
        (is_prediction ? "true" : "false"));
    json.append("}");

    return json;
}

User::~User()
{
    for (unsigned i = 0; i < events->size(); i++) {
        free(events->at(i));
    }
    delete events;
}

void User::add_event(Event* event)
{
    assert(event->user_id == get_id());

    Event* new_event = (Event*)malloc(sizeof(Event));
    new_event->user_id = get_id();
    new_event->antenna_id = event->antenna_id;
    new_event->day = event->day;
    new_event->hour = event->hour;

    events->push_back(new_event);
    std::cout << to_json(new_event, false) << std::endl;

    set_dirty();
}

void User::addEvent(valarray<int> newEvent) {
    // TODO: would it be easier to make it work with add_event i.e. using Event*, whichd
    // packages date/time info for you?
    Event event;
    to_event(newEvent, &event);
    add_event(&event);

    //number of seconds is always zero
    times.push_back(newEvent[EV_HOUR]*60+newEvent[EV_MINUTE]);
    original.push_back(antennas.getCopy(0,newEvent[EV_ANTENNA]));
    set_dirty();
}

void User::smooth() {
    smoothedUpToDate=true;
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
    return events->back();
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
	for (unsigned i = 0; i < times.size(); ++i) {
      zipped.push_back(ii(times[i],i));
  }
	sort(zipped.begin(), zipped.end(), comparison);
	//The following slow search doesn't affect running time since we just did a sort which is slower
	unsigned i = 0;
	for (; i<zipped.size(); ++i) {
      if (zipped[i].first > (int)after_time) break;
  }
	*out_time=zipped[i].first;
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
        new_predicted_event->day = day;
        new_predicted_event->hour = hour;
        std::cout << to_json(new_predicted_event, true) << std::endl;
    }
}
