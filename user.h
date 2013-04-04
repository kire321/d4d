#include <event.h>

class User {
	Table calls;
	//we need to store both smoothed and original locations
public:
	void addEvent(Event newEvent);
	void smooth();
	Event makePrediction();
};