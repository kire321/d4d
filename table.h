#include <iostream>
#include <unordered_map>
#include <assert.h>
#include <valarray>
#include <string>
#include <vector>
#include <boost\any.hpp>

#pragma once

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