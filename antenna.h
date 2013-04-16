#include "types.h"

#include "multiDimVala.h"

class Antenna
{
    public:
        multiDimVala<float> *antennas;
        int index;

        Antenna(multiDimVala<float> *argAntennas, int argIndex) : antennas(argAntennas), index(argIndex) {assert(antennas->shape[1]==2);}

        float lattitude() const { return antennas->getSingle(index,0); };
        float longitude() const { return antennas->getSingle(index,1); };
        AntennaId get_id();

        float distance_from(float lat, float lon);

        Antenna nearest(multiDimVala<float> &otherAntennas); // TODO: get rid

        void print() {cout << lattitude() << "\t" << longitude() << endl;}
        bool operator==(const Antenna& rhs)
        {
            return this->lattitude() == rhs.lattitude() &&
                this->longitude() == rhs.longitude();
        };
        bool operator!=(const Antenna& rhs) { return !(*this == rhs); };
};
