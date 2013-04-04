#include <antenna.h>

Table *Antenna::antennas=NULL;

Antenna::Antenna(int index):
	latitude(antennas->getColumn<float>("latitude")[index]),
	longitude(antennas->getColumn<float>("longitude")[index]) {}