#pragma once

#include <cmath>

#include "types.h"

class Antenna
{
    float latitude;
    float longitude;
    AntennaId id;

    public:
        Antenna(float lat, float lon, AntennaId aid) : latitude(lat),
            longitude(lon), id(aid) {};

        AntennaId get_id() const { return id; };
        float get_latitude() const { return latitude; };
        float get_longitude() const { return longitude; };

        float distance_from(float lat, float lon)
        {
            return (pow(fabs(get_latitude() - lat), 2) +
                pow(fabs(get_longitude() - lon), 2));
        }

        bool operator==(const Antenna& rhs)
        {
            return (get_latitude() == rhs.get_latitude() &&
                get_longitude() == rhs.get_longitude());
        }
        bool operator!=(const Antenna& rhs) { return !(*this == rhs); };
};
