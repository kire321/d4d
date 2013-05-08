#pragma once

#include <string>
#include <boost/math/distributions/normal.hpp>
#include <algorithm>
#include <vector>
#include <valarray>

#include "types.h"
#include "path.h"

using namespace boost::math;
using std::valarray;
using std::string;

class User
{
    UserId id;
    vector<Event*> events;
    Event* last_event;

    public:
        static void to_event(valarray<int> event_data, Event* event);
        static string to_json(Event* event, bool is_prediction = true);
        static int to_minutes(time_duration duration);
        static bool earlier_event_time(Event* a, Event* b);

        User(UserId id) : id(id), last_event(NULL) {};
        ~User();

        void add_event(Event* event);

        UserId get_id() const { return id; };
        vector<Event*> get_events() const { return events; };
        Event* get_last_event() const { return last_event; };

        void next_likely_event(Event* after_event, Event* likely_event);
        void previous_event(Event* previous_event);

    private:
        AntennaId get_smoothed_antenna(time_duration time);
};
