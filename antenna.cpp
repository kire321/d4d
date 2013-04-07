#include <antenna.h>

multiDimVala<float>& Antenna::antennas=multiDimVala<float>();

Antenna Antenna::nearest() {
	multiDimVala<float> deltas=antennas-view;
	deltas.data*=deltas.data;
	return Antenna(argmin(deltas.sum(1)));
};