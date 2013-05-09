#pragma once

#include <valarray>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "types.h"

using namespace boost;
using boost::gregorian::date;
using boost::gregorian::date_duration;
using std::valarray;
using std::vector;
using std::string;
using std::istream;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

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

void to_event(valarray<int> event_data, Event* event);
int read_event(Event* event, istream& file);
string to_json(Event* event, bool is_prediction);
int to_minutes(time_duration duration);
bool earlier_event_time(Event* a, Event* b);
Event* event_copy(Event* other);
