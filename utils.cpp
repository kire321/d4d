#include <sstream>

#include "utils.h"

using std::cerr;
using std::fstream;
using std::stringstream;
using std::cerr;
using std::endl;

void to_event(valarray<int> event_data, Event* event)
{
    event->user_id = event_data[EV_UID];
    event->antenna_id = event_data[EV_ANTENNA];
    event->time = ptime(date(event_data[EV_YEAR], event_data[EV_MONTH],
        event_data[EV_DAY]), time_duration(event_data[EV_HOUR],
        event_data[EV_MINUTE], event_data[EV_SECOND]));
    // NOTE: Can't use time_from_string because we're passing a valarray of ints
}

int read_event(Event* event, istream& file)
{
    string line;
    getline(file,line);
    valarray<int> splitLine;
    try {
        splitLine=splitConvert<int>(line,"\t :-");
        if (splitLine[EV_UID] == CLEAR_USERS) {
            return CLEAR_USERS;
        }
    } catch (...) {
        cerr << "Failed to parse a line of events\n";
        return ERR;
    }
    if (splitLine[EV_ANTENNA]!=-1) {
        to_event(splitLine, event);
        if (LOG) cerr << "Converted to event";
    }
    return splitLine[EV_ANTENNA];
}

string to_json(Event* event, bool is_prediction)
{
    stringstream json;
    json << "{" << endl;
    json << "\t\"uid\" : " << event->user_id << "," << endl;
    json << "\t\"antenna\" : " << event->antenna_id << "," << endl;
    json << "\t\"time\" : \"" << to_simple_string(event->time) << "\"," << endl;
    json << "\t\"is_prediction\" : " << (is_prediction ? "true" : "false")
        << endl;
    json << "}";

    return json.str();
}

int to_minutes(time_duration duration)
{
    return duration.total_seconds() / 60;
}

bool earlier_event_time(Event* a, Event* b)
{
    return a->time.time_of_day() <= b->time.time_of_day();
}

Event* event_copy(Event* other)
{
    Event* new_event = (Event*)malloc(sizeof(Event));
    new_event->user_id = other->user_id;
    new_event->antenna_id = other->antenna_id;
    new_event->time = other->time;
    return new_event;
}
