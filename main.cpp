#include <user.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <antenna.h>
using namespace boost;

template <class T>
valarray<T> splitConvert(string line, const char *seperator) {
	vector<string> splitVec;
	split(splitVec,line,is_any_of(seperator),token_compress_on);
	valarray<T> toret(splitVec.size());
	for(int i=0; i<splitVec.size(); ++i) {
		toret[i]=lexical_cast<T>(splitVec[i]);
	}
	return toret;
}

vector<valarray<float>> parseAntennas(char *fname) {
	ifstream file;
	string line;
	vector<valarray<float>> toret;
	try {
			file.open(fname);
	}
	catch (...) {
		cout << "Could not open file " << fname << ". Skipping file." << endl;
	}
	while(file.good()) {
		getline(file,line);
		try {
			toret.push_back(splitConvert<float>(line,"\t")[slice(1,2,1)]);
		}
		catch (...) {
			cout << "Failed to parse a line in antenna file. Continuing." << endl;
		}
	}
	file.close();
	return toret;
}

vector<valarray<int>> parseEvents(int argc, char *argv[], int maxEvents) {
	int counter=0;
	ifstream file;
	string line;
	bool breaking=false;
	vector<valarray<int>> events;
	
	for(int i=2; i<argc; ++i) {
		try {
			file.open(argv[i]);
		}
		catch (...) {
			cout << "Could not open file " << __argv[i] << ". Skipping file." << endl;
		}
		while(file.good()) {
			getline(file,line);
			valarray<int> splitLine;
			try {
				splitLine=splitConvert<int>(line,"\t :-");
			}
			catch (...) {
				cout << "Failed to parse a line of " << __argv[i] << ". Continuing." << endl;
				continue;
			}
			if(splitLine[EV_ANTENNA]!=-1)
				events.push_back(splitLine);
			if(++counter>=maxEvents) {
				breaking=true;
				break;
			}
		}
		file.close();
		if(breaking)
			break;
	}
	return events;
}		

int main(int __argc, char* __argv[]) {
	//first argument is antenna table, all other arguments list event tables
	//print first 100 events and their locations
	if(__argc < 3) {
		cout << "We need at least two arguments: the antennas and at least one file with events." << endl;
		return 0;
	}
	int nEvents=100;
	
	multiDimVala<float> antennas(parseAntennas(__argv[1]));
	multiDimVala<int> events(parseEvents(__argc, __argv, nEvents));
	//The data wasn't formatted by a programmer because it starts counting from 1.
	events.getView(1,EV_ANTENNA)-=valarray<int>(1,nEvents);
	events.getView(1,EV_UID)-=valarray<int>(1,nEvents);
	User::antennas=antennas;
	Antenna::antennas=antennas;
	vector<User> users;
	for(int i=0; i<nEvents; ++i) {
		valarray<int> cur=events.getCopy(0,i);
		int uid=cur[EV_UID];
		//for testing
		if(uid==0)
			Antenna(cur[EV_ANTENNA]).print();
		//end for testing
		if(uid >= users.size())
			users.push_back(User());
		users[uid].addEvent(cur);
	}
	
	cout << endl << endl << endl;
	multiDimVala<float> u1=users[0].getSmoothed();
	for(int i=0; i<(int) u1.shape[0]; ++i)
		Antenna(u1.getView(0,i)).nearest().print();

	string line;
	cin >> line;
	return 0;
}