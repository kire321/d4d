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
    bool smoothedUpToDate;
    void smooth();
    vector<Event*> predictions;

    vector<Event*> events;

    public:
        static void to_event(valarray<int> event_data, Event* event);
        static string to_json(Event* event, bool is_prediction = true);

        User(UserId id) : id(id), smoothedUpToDate(false) {};
        ~User();

        void add_event(Event* event);

        UserId get_id() const { return id; };
        vector<Event*> get_events() const { return events; };

        void set_dirty() { smoothedUpToDate = false; };
        bool is_dirty() const { return smoothedUpToDate != true; };

        void next_likely_location(unsigned after_time, unsigned* out_time, AntennaId *out_ant);

        multiDimVala<float> getSmoothed();
        Event* get_last_event();

        void make_prediction(Path& predicted_path, unsigned day, unsigned time);
};
