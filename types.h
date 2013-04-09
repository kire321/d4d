#pragma once

typedef unsigned UserId;
typedef unsigned AntennaId;

typedef struct event {
    UserId user_id;
    AntennaId antenna_id;
    unsigned time;
} Event;
