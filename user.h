#include <multiDimVala.h>
#include <iostream>
#include <string>
#include <boost\math\distributions\normal.hpp>
#pragma once
using namespace boost::math;

#define EV_UID 0
#define EV_YEAR 1
#define EV_MONTH 2
#define EV_DAY 3
#define EV_HOUR 4
#define EV_MINUTE 5
#define EV_SECOND 6
#define EV_ANTENNA 7
#define EV_SIZE 8

class User {
	vector<int> times;
	vector<valarray<float>> original;
	multiDimVala<float> smoothed;
	bool smoothedUpToDate;
	void smooth();
public:
	User(): smoothedUpToDate(false) {}
	static multiDimVala<float>& antennas;
	void addEvent(valarray<int> newEvent);
	multiDimVala<float> getSmoothed();
	/*Event makePrediction();*/
};