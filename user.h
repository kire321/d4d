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
    vector<Event> predictions;
    vector<Event*> events;

    public:
        static void to_event(valarray<int> event_data, Event* event);
        static string to_json(Event* event, bool is_prediction = true);

        User(UserId id) : id(id) {};
        ~User();

        void add_event(Event* event);

        UserId get_id() const { return id; };
        vector<Event*> get_events() const { return events; };

        void next_likely_location(unsigned after_time, unsigned* out_time, AntennaId *out_ant);

        Event* get_last_event();

        void make_prediction(Path& predicted_path, unsigned day, unsigned time);
};
