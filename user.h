#include <iostream>
#include <assert.h>
#include <valarray>
#include <string>
#include <vector>
#pragma once
using namespace std;

#define EV_UID 0
#define EV_YEAR 1
#define EV_MONTH 2
#define EV_DAY 3
#define EV_HOUR 4
#define EV_MINUTE 5
#define EV_SECOND 6
#define EV_ANTENNA 7
#define EV_SIZE 8

template <class T> valarray<T> vecValToVal(vector<valarray<T>> vvT);

class User {
	vector<int> times;
	vector<valarray<float>> original;
	valarray<float> smoothed;
	bool smoothedUpToDate;
	void smooth();
public:
	User(): smoothedUpToDate(false) {}
	static valarray<float>& antennas;
	void addEvent(valarray<int> newEvent);
	valarray<float> getSmoothed();
	/*Event makePrediction();*/
};