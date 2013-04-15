#pragma once

#include <string>
#include <boost/math/distributions/normal.hpp>

#include "multiDimVala.h"
#include "types.h"
#include <vector>

using namespace boost::math;

enum EV { EV_UID = 1, EV_YEAR, EV_MONTH, EV_DAY, EV_HOUR, EV_MINUTE, EV_SECOND,
    EV_ANTENNA, EV_SIZE };

class User
{
    UserId id;
    vector<int> times;
    vector<valarray<float> > original;
    multiDimVala<float> smoothed;
    bool smoothedUpToDate;
    void smooth();

    vector<Event*>* events;

    public:
        static void to_event(valarray<float> event_data, Event* event);
        static multiDimVala<float> antennas; // FIXME: what does this do?

        User(UserId id) : id(id), smoothedUpToDate(false) { events = new vector<Event*>(); };
        ~User();

        void addEvent(valarray<int> newEvent);
        void add_event(Event* event);

        void get_id() const { return id; };

        void set_dirty() { smoothedUpToDate = false; };
        void is_dirty() { smoothedUpToDate != true; };

        AntennaId next_likely_location(unsigned after_time);

        multiDimVala<float> getOriginal() {return multiDimVala<float>(original);}
        multiDimVala<float> get
        Event* get_last_event();
        /*Event makePrediction();*/
};
