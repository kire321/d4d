#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <valarray>
using namespace std;
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
	file.open(fname);
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

template <class T>
void printVV(vector<valarray<T>> toprint) {
	for(int i=0; i<toprint.size(); ++i) {
		for(int j=0; j<toprint[i].size(); ++j) {
			cout << toprint[i][j] << "\t";
		}
		cout << endl;
	}
}

int main(int __argc, char* __argv[]) {
	//first argument is antenna table, all other arguments list event tables
	//print first 100 events and their locations
	if(__argc < 3) {
		cout << "We need at least two arguments: the antennas and at least one file with events." << endl;
		return 0;
	}
	
	int counter=0;
	ifstream file;
	string line;
	bool breaking=false;
	vector<valarray<float>> antennas=parseAntennas(__argv[1]);
	vector<valarray<int>> events;
	
	for(int i=2; i<__argc; ++i) {
		file.open(__argv[i]);
		while(file.good()) {
			getline(file,line);
			try {
				events.push_back(splitConvert<int>(line,"\t :-"));
			}
			catch (...) {
				cout << "Failed to parse a line of " << __argv[i] << ". Continuing." << endl;
				continue;
			}
			if(counter++>=100) {
				breaking=true;
				break;
			}
		}
		file.close();
		if(breaking)
			break;
	}
	
	printVV<int>(events);

	cin >> line;
	return 0;
}