#pragma once

#include <string>
#include <boost/math/distributions/normal.hpp>

#include "multiDimVala.h"
#include "types.h"

using namespace boost::math;

enum EV { EV_UID = 1, EV_YEAR, EV_MONTH, EV_DAY, EV_HOUR, EV_MINUTE, EV_SECOND,
    EV_ANTENNA, EV_SIZE };

class User {
    vector<int> times;
    vector<valarray<float> > original;
    multiDimVala<float> smoothed;
    bool smoothedUpToDate;
    void smooth();

    public:
        static multiDimVala<float> antennas; // FIXME: what does this do?

        User() : smoothedUpToDate(false) {}

        void addEvent(valarray<int> newEvent);

        multiDimVala<float> getOriginal() {return multiDimVala<float>(original);}
        multiDimVala<float> getSmoothed();
        Event* get_last_event();
        /*Event makePrediction();*/
};
