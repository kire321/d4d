#include "user.h"
#include "utils.h"
#include <cstdlib> // TODO: remove me
#include <assert.h>

multiDimVala<float> User::antennas = multiDimVala<float>();

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


void User::next_likely_location(unsigned after_time, unsigned *out_time, AntennaId *out_ant)
{
	vector<ii> zipped;
	for(int i=0; i<times.size(); ++i)
		zipped.push_back(ii(times[i],i));
	sort(zipped.begin(), zipped.end(), comparison);
	//The following slow search doesn't affect running time since we just did a sort which is slower
	int i=0;
	for(; i<zipped.size(); ++i)
		if(zipped[i].first>after_time)
			break;
	*out_time=zipped[i].first;
	valarray<float> latLon=getSmoothed().getCopy(0,zipped[i].second);
	*out_ant=g_antenna_model.find_nearest_antenna(latLon[0],latLon[1])->get_id();
}

void User::make_prediction(Path& predicted_path, unsigned day, unsigned hour)
{
    AntennaId predicted_antenna = predicted_path.get_next_step(true);
    while (predicted_antenna > 0) { // Is a valid antenna
        Event* new_predicted_event = (Event*)malloc(sizeof(Event));
        new_predicted_event->user_id = id;
        new_predicted_event->antenna_id = predicted_antenna;
        new_predicted_event->day = day;
        new_predicted_event->hour = hour;
        std::cout << to_json(new_predicted_event, true) << std::endl;
    }
}
