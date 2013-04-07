#include <multiDimVala.h>

class Antenna {
public:
	slice_array<float> view;
	static multiDimVala<float>& antennas;
	Antenna(slice_array<float> argView): view(argView) {assert(view.size()==2);}
	Antenna(int index): view(antennas.getView(0,index)) {}
	float lat() const {return valarray<float>(view)[0];}
	float lon() const {return valarray<float>(view)[1];}
	void print() {cout << lat() << "\t" << lon() << endl;}
	Antenna nearest();
	bool operator==(const Antenna& rhs) {return this->lat()==rhs.lat() && this->lon()==rhs.lon();}
	bool operator!=(const Antenna& rhs) {return !(*this==rhs);}
};