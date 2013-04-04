#include <table.h>

class Antenna {
public:
	//Right now you can't subtract two antennas to find the vector between them without. Thouughts an a good workaround?
	//Would we be willing to sacrifice that changes to the antenna object modify the table? Or two classes, antennaRef and antennaForMath?
	static Table *antennas;
	float &latitude;
	float &longitude;
	Antenna(int index);
};