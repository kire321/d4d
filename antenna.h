#include "multiDimVala.h"

class Antenna {
    public:
        multiDimVala<float> *antennas;
        int index;
        Antenna(multiDimVala<float> *argAntennas, int argIndex): antennas(argAntennas), index(argIndex) {assert(antennas->shape[1]==2);}
        float lat() const {return antennas->getSingle(index,0);}
        float lon() const {return antennas->getSingle(index,1);}
        void print() {cout << lat() << "\t" << lon() << endl;}
        Antenna nearest(multiDimVala<float> &otherAntennas);
        bool operator==(const Antenna& rhs) {return this->lat()==rhs.lat() && this->lon()==rhs.lon();}
        bool operator!=(const Antenna& rhs) {return !(*this==rhs);}
};
