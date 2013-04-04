#include <iostream>
#include <unordered_map>
#include <assert.h>
#include <valarray>
#include <string>
#include <vector>
#include <boost\any.hpp>
using namespace boost;
using namespace std;

class Table {
	unordered_map<string,valarray<any>> columns;
public:
	Table(vector<string> metadata, vector<valarray<any>> copyFrom);
	void addColumn(string name, valarray<any> column) {columns.insert(pair<string,valarray<any>>(name,column));}
	void deleteColumn(string name);
	int size() {return columns.begin()->second.size();}
	template<class T> valarray<T> getColumn(string name);
	void merge(string indexName, Table other);
	void print();
};

class Antenna {
public:
	//Right now you can't subtract two antennas to find the vector between them without. Thouughts an a good workaround?
	//Would we be willing to sacrifice that changes to the antenna object modify the table? Or two classes, antennaRef and antennaForMath?
	static Table *antennas;
	float &latitude;
	float &longitude;
	Antenna(int index);
};

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

class User {
	Table calls;
	//we need to store both smoothed and original locations
public:
	void addEvent(Event newEvent);
	void smooth();
	Event makePrediction();
};