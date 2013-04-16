#include "antenna.h"

Antenna Antenna::nearest(multiDimVala<float> &otherAntennas) {
    multiDimVala<float> deltas=otherAntennas-antennas->getCopy(0,index);
    deltas.data*=deltas.data;
    return Antenna(&otherAntennas,argmin(deltas.sum(1)));
};

float Antenna::distance_from(float lat, float lon)
{
    return sqrt(fabs(get_latitude() - latitude) + fabs(get_longitude() -
        longitude));
}
