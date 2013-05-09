#pragma once

#include <string>
#include <boost/math/distributions/normal.hpp>
#include <vector>
#include <valarray>
#include <map>

#include "types.h"
#include "path.h"

using namespace boost::math;
using std::valarray;
using std::string;
using std::map;

class User
{
    UserId id;
    map<AntennaId, vector<AntennaId> > transitions;
    vector<Event*> events;
    Event* last_event;

    public:
        User(UserId id) : id(id), last_event(NULL) { transitions = map<AntennaId, vector<AntennaId> >(); };
        ~User();

        void add_event(Event* event);

        UserId get_id() const { return id; };
        vector<Event*> get_events() const { return events; };
        Event* get_last_event() const { return last_event; };

        AntennaId make_prediction(ptime time);
        int estimate_num_steps(ptime start, ptime end);

    private:
        void previous_event(Event* previous_event);
        int step_size_at(time_duration time);
};
