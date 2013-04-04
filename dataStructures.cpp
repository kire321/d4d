#include <dataStructures.h>

Table *Antenna::antennas=NULL;
Table *Event::events=NULL;

Table::Table(vector<string> metadata, vector<valarray<any>> copyFrom) {
	assert(copyFrom.size()>=1);
	assert(metadata.size()==copyFrom[0].size());
	//We don't check that all valarrays are the same size
	for(int i=0; i<metadata.size(); ++i) {
		valarray<any> column(copyFrom.size());
		for(int j=0; j<copyFrom.size(); ++j) {
			column[j]=copyFrom[j][i];
		}
		addColumn(metadata[i],column);
	}
}

void Table::deleteColumn(string name) {
	int status=columns.erase(name);
	if(status==0)
		throw "Nothing by that name to erase.";
}

template<class T>
valarray<T> Table::getColumn(string name) {
	valarray<T> toret(size());
	for(int i=0; i<size(); ++i)
		toret[i]=any_cast<T>(columns[name][i]);
	return toret;
}

void Table::merge(string indexName, Table other) {
	valarray<int> indices=getColumn<int>(indexName);
	for(unordered_map<string,valarray<any>>::iterator it=other.columns.begin(); it!=other.columns.end(); ++it) {
		valarray<any> newColumn(size());
		valarray<any> otherColumn=it->second;
		for(int i=0; i<size(); ++i)
			newColumn[i]=otherColumn[indices[i]];
		addColumn(it->first,newColumn);
	}
	deleteColumn(indexName);
}

void Table::print() {
	for(unordered_map<string,valarray<any>>::iterator it=columns.begin(); it!=columns.end(); ++it)
		cout << it->first << "\t";
	cout << endl;
	for(int i=0; i<size(); ++i) {
		for(unordered_map<string,valarray<any>>::iterator it=columns.begin(); it!=columns.end(); ++it) {
			if(it->second[0].type()==typeid(int))
				cout << getColumn<int>(it->first)[i] << "\t";
			else if(it->second[0].type()==typeid(float))
				cout << getColumn<float>(it->first)[i] << "\t";
			else
				throw "Could not print a column.";
		}
		cout << endl;
	}
}

Antenna::Antenna(int index):
	latitude(antennas->getColumn<float>("latitude")[index]),
	longitude(antennas->getColumn<float>("longitude")[index]) {}

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