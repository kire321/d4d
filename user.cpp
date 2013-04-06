#include <user.h>

//our data is from a leap year
int monthLengths[12]={31,29,31,30,31,30,31,31,30,31,30,31};
valarray<float>& User::antennas=valarray<float>();

template <class T>
valarray<T> vecValToVal(vector<valarray<T>> vvT) {
	valarray<T> toret(vvT.size()*vvT[0].size());
	for(int i=0; i<vvT.size(); ++i)
		toret[slice(i*vvT[0].size(),vvT[0].size(),1)]=vvT[i];
	return toret;
}

void User::addEvent(valarray<int> newEvent) {
	//number of seconds is always zero
	times.push_back(((newEvent[EV_YEAR]-2010)*365+monthLengths[newEvent[EV_MONTH]]+newEvent[EV_DAY])*24*60+newEvent[EV_HOUR]*60+newEvent[EV_MINUTE]);
	original.push_back(antennas[slice(newEvent[EV_ANTENNA],2,1)]);
}

void User::smooth() {
	smoothedUpToDate=true;
	//TODO: actual implementation
	smoothed=vecValToVal<float>(original);
}

valarray<float> User::getSmoothed() {
	if(!smoothedUpToDate)
		smooth();
	return smoothed;
}