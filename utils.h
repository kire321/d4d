#pragma once

#include <valarray>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;

template <class T>
valarray<T> splitConvert(string line, const char *seperator)
{
    vector<string> splitVec;
    split(splitVec,line,is_any_of(seperator),token_compress_on);
    valarray<T> toret(splitVec.size());
    for(unsigned i=0; i<splitVec.size(); ++i) {
        toret[i]=lexical_cast<T>(splitVec[i]);
    }
    return toret;
}

void to_event(valarray<int> event_data, Event* event)
{
    event->user_id = event_data[EV_UID];
    event->antenna_id = event_data[EV_ANTENNA];
    event->day = (event_data[EV_YEAR] - 2000) * 10000 +
        event_data[EV_MONTH] * 100 + EV_DAY; // Hack to get unique days
    event->hour = event_data[EV_HOUR];
}

string to_json(Event* event, bool is_prediction)
{
}
