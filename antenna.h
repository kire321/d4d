#pragma once

#include "types.h"
#include "multiDimVala.h"

class Antenna
{
    float latitude;
    float longitude;

    public:
        Antenna(float lat, float lon, int id) : latitude(lat), longitude(lon), index(id) {};

        AntennaId get_id() const { return index; };
        float get_latitude() const { return latitude; };
        float get_longitude() const { return longitude; };

        float distance_from(float lat, float lon);

        // Original implementation
        multiDimVala<float> *antennas;
        int index;

        Antenna(multiDimVala<float> *argAntennas, int argIndex) : antennas(argAntennas), index(argIndex) {assert(antennas->shape[1]==2);}

        float lat() const { return antennas->getSingle(index,0); };
        float lon() const { return antennas->getSingle(index,1); };

        Antenna nearest(multiDimVala<float> &otherAntennas); // TODO: get rid

        void print() {cout << lat() << "\t" << lon() << endl;}
        bool operator==(const Antenna& rhs)
        {
            return this->lat() == rhs.lat() && this->lon() == rhs.lon();
        };
        bool operator!=(const Antenna& rhs) { return !(*this == rhs); };
};
