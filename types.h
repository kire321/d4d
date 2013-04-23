#pragma once

typedef int UserId;
typedef int AntennaId;

typedef struct event {
    UserId user_id;
    AntennaId antenna_id;
    int day;
    int hour;
} Event;

// FIXME unsigned -> signed int
