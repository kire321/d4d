#pragma once

typedef int UserId;
typedef int AntennaId;

typedef struct event {
    UserId user_id;
    AntennaId antenna_id;
    int day;
    int hour;
} Event;

enum EV { EV_UID = 0, EV_YEAR, EV_MONTH, EV_DAY, EV_HOUR, EV_MINUTE, EV_SECOND,
    EV_ANTENNA, EV_SIZE };

// FIXME unsigned -> signed int
