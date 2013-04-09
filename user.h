#include <multiDimVala.h>
#include <string>
#include <boost\math\distributions\normal.hpp>
#pragma once
using namespace boost::math;

enum EV { EV_UID = 1, EV_YEAR, EV_MONTH, EV_DAY, EV_HOUR, EV_MINUTE, EV_SECOND,
    EV_ANTENNA, EV_SIZE }

class User {
    vector<int> times;
    vector<valarray<float>> original;
    multiDimVala<float> smoothed;
    bool smoothedUpToDate;
    void smooth();
public:
    User(): smoothedUpToDate(false) {}
    static multiDimVala<float>& antennas;
    void addEvent(valarray<int> newEvent);
    multiDimVala<float> getOriginal() {return multiDimVala<float>(original);}
    multiDimVala<float> getSmoothed();
    /*Event makePrediction();*/
};
