#include <user.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>
using namespace boost;

template <class T>
valarray<T> vecValToVal(vector<valarray<T>> vvT) {
	valarray<T> toret(vvT.size()*vvT[0].size());
	for(int i=0; i<vvT.size(); ++i)
		toret[slice(i*vvT[0].size(),vvT[0].size(),1)]=vvT[i];
	return toret;
}

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
			try {
				events.push_back(splitConvert<int>(line,"\t :-"));
			}
			catch (...) {
				cout << "Failed to parse a line of " << __argv[i] << ". Continuing." << endl;
				continue;
			}
			if(counter++>=maxEvents) {
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
	int nEvents=1000;
	
	valarray<float> antennas=vecValToVal<float>(parseAntennas(__argv[1]));
	valarray<int> events=vecValToVal<int>(parseEvents(__argc, __argv, nEvents));
	events[slice(EV_ANTENNA,nEvents,EV_SIZE)]-=valarray<int>(1,nEvents);
	events[slice(EV_UID,nEvents,EV_SIZE)]-=valarray<int>(1,nEvents);
	User::antennas=antennas;
	vector<User> users;
	for(int i=0; i<nEvents; ++i) {
		int uid=events[i*EV_SIZE+EV_UID];
		if(uid >= users.size())
			users.push_back(User());
		users[uid].addEvent(events[slice(i,EV_SIZE,1)]);
	}
	for(int i=0; i<users.size(); ++i)
		cout << users[i].getSmoothed().size() << endl;

	string line;
	cin >> line;
	return 0;
}