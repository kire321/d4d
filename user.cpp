#include "user.h"

multiDimVala<float> User::antennas = multiDimVala<float>();

void User::addEvent(valarray<int> newEvent) {
    //number of seconds is always zero
    times.push_back(newEvent[EV_HOUR]*60+newEvent[EV_MINUTE]);
    original.push_back(antennas.getCopy(0,newEvent[EV_ANTENNA]));
    smoothedUpToDate=false;
}

void User::smooth() {
    smoothedUpToDate=true;
    multiDimVala<float> vala(original);
    smoothed=multiDimVala<float>(original.size(),2);
    for (unsigned i=0; i<original.size(); ++i) {
        multiDimVala<float> gauss(times.size(),2);
        for (unsigned j=0; j<times.size(); ++j)
            gauss.getView(0,j)=pdf(normal(times[i],60),times[j]);
        valarray<float> denom=gauss.sum(0);
        gauss.data*=vala.data;
        smoothed.getView(0,i)=gauss.sum(0)/denom;
    }    
}

multiDimVala<float> User::getSmoothed() {
    if (!smoothedUpToDate)
        smooth();
    return smoothed;
}
