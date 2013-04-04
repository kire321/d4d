#include <dataStructures.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

template <class T>
valarray<any> splitConvert(string line, const char *seperator) {
	vector<string> splitVec;
	split(splitVec,line,is_any_of(seperator),token_compress_on);
	valarray<any> toret(splitVec.size());
	for(int i=0; i<splitVec.size(); ++i) {
		toret[i]=lexical_cast<T>(splitVec[i]);
	}
	return toret;
}

vector<valarray<any>> parseAntennas(char *fname) {
	ifstream file;
	string line;
	vector<valarray<any>> toret;
	try {
			file.open(fname);
	}
	catch (...) {
		cout << "Could not open file " << fname << ". Skipping file." << endl;
	}
	while(file.good()) {
		getline(file,line);
		try {
			toret.push_back(splitConvert<float>(line,"\t"));
		}
		catch (...) {
			cout << "Failed to parse a line in antenna file. Continuing." << endl;
		}
	}
	file.close();
	return toret;
}

vector<valarray<any>> parseEvents(int argc, char *argv[], int maxEvents) {
	int counter=0;
	ifstream file;
	string line;
	bool breaking=false;
	vector<valarray<any>> events;
	
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

	vector<string> antennaMetadata;
	antennaMetadata.push_back("antennaID");
	antennaMetadata.push_back("latitude");
	antennaMetadata.push_back("longitude");
	Table antennas(antennaMetadata,parseAntennas(__argv[1]));
	Antenna::antennas=&antennas;
	antennas.deleteColumn("antennaID");

	vector<string> eventMetadata;
	eventMetadata.push_back("userID");
	eventMetadata.push_back("year");
	eventMetadata.push_back("month");
	eventMetadata.push_back("day");
	eventMetadata.push_back("hour");
	eventMetadata.push_back("minute");
	eventMetadata.push_back("second");
	eventMetadata.push_back("antennaID");
	Table events(eventMetadata,parseEvents(__argc, __argv, 100));
	Event::events=&events;
	cout << events.getColumn<int>("antennaID")[0] << endl;
	events.getColumn<int>("antennaID")-=1;
	cout << events.getColumn<int>("antennaID")[0] << endl;
	events.merge("antennaID",antennas);
	events.print();
	
	string line;
	cin >> line;
	return 0;
}