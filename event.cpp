#include <event.h>

Table *Event::events=NULL;

Event::Event(int index):
	latitude(events->getColumn<float>("latitude")[index]),
	longitude(events->getColumn<float>("longitude")[index]),
	userID(events->getColumn<int>("userID")[index]),
	year(events->getColumn<int>("year")[index]),
	month(events->getColumn<int>("month")[index]),
	day(events->getColumn<int>("day")[index]),
	hour(events->getColumn<int>("hour")[index]),
	minute(events->getColumn<int>("minute")[index]),
	second(events->getColumn<int>("second")[index]) {}