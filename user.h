#pragma once

#include <string>
#include <boost/math/distributions/normal.hpp>
#include <algorithm>

#include "multiDimVala.h"
#include "types.h"
#include "path.h"
#include <vector>

using namespace boost::math;

class User
{
    UserId id;
    multiDimVala<float> smoothed;
    void smooth();
    vector<Event*> predictions;

    vector<Event*> events;
    unsigned num_unsmoothed_events; // Only smooth once we have 10 new events

    public:
        static void to_event(valarray<int> event_data, Event* event);
        static string to_json(Event* event, bool is_prediction = true);

        User(UserId id) : id(id), num_unsmoothed_events(0) {};
        ~User();

        void add_event(Event* event);

        UserId get_id() const { return id; };
        vector<Event*> get_events() const { return events; };

        void set_dirty() { num_unsmoothed_events++; };
        // TODO: make a constant so we can tweak the 'dirty' threshold
        bool is_dirty() const { return num_unsmoothed_events >= 10; };

        void next_likely_location(unsigned after_time, unsigned* out_time, AntennaId *out_ant);

        multiDimVala<float> getSmoothed();
        Event* get_last_event();

        void make_prediction(Path& predicted_path, unsigned day, unsigned time);
};
