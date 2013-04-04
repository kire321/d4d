#include <table.h>

class Event {
public:
	static Table *events;
	int &userID;
	int &year;
	int &month;
	int &day;
	int &hour;
	int &minute;
	int &second;
	float &latitude;
	float &longitude;
	Event(int index);
};