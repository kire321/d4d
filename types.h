#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

typedef int UserId;
typedef int AntennaId;

typedef struct event {
    UserId user_id;
    AntennaId antenna_id;
    ptime time;
} Event;

enum EV { EV_UID = 0, EV_YEAR, EV_MONTH, EV_DAY, EV_HOUR, EV_MINUTE, EV_SECOND,
    EV_ANTENNA, EV_SIZE };

#define CLEAR_USERS (-1)
#define ERR (INT_MIN)

#define LOG 0
