typedef UserId unsigned;
typedef AntennaId unsigned;

typedef struct event {
    UserId user_id;
    AntennaId antenna_id;
    unsigned time;
} Event;
