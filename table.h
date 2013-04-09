/*#include <iostream>
#include <unordered_map>
#include <assert.h>
#include <valarray>
#include <string>
#include <vector>

#pragma once

using namespace boost;
using namespace std;

class TypeData {
public:
	int size;
	type_info typeInfo;
	template <class T> TypeData(): size(sizeof(T)), typeInfo(typeID(T)) {}
};

class Table {
	unordered_map<string,pair<TypeData,void *>> columns;
	int size;
	int capacity;
public:
	Table(): size(1) {}
	int getSize() {return size;}
	int getCapacity() {return capacity;}
	template <class T> void addColumn(string name, valarray<T> column);
	void addRow();
	template <class T> T& getFromLastRow(string name);
	void deleteColumn(string name);
	template<class T> valarray<T>& operator[](string name);
	void merge(string indexName, Table other);
	void print();
};*/