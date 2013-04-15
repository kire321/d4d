#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <boost/any.hpp>
#include <assert.h>
#include <boost/algorithm/string.hpp>
#include <unordered_set>
using namespace std;
using namespace boost;
#pragma once

class Column {
public:
    bool thisTree;
    vector<vector<int> *> indirections;
    vector<any> *data;
    size_t typeHash;
    Column(): typeHash(0) {}
    template <class T> void setHash();
};

class Slice {
public:
    int begin,end,step;
    Slice(int argB, int argE, int argS): begin(argB), end(argE), step(argS) {}
    Slice(int argB, int argE): begin(argB), end(argE), step(1) {}
    Slice(int argB): begin(argB), end(argB+1), step(1) {}
};

typedef unordered_map<string,Column> colmap;

class Table {
    colmap columns;
    vector<int> indirection;
    template <class T> Table& deepCopy();
public:
    void addRow(); //adds a row of default-constructed objects to base data and adds an index to it all views between *this and root. Views on other branches are unaffected.
    int size() const;
    template <class T> void addCol(string name); //adds a column of default constructed anys. Does not effect tables higher than *this on the tree of views; *this owns the data. The type of the column is decided when you add the first element.
    Table(const Table& other); //the copies refer to the same information as *this, not to *this
    Table(const colmap &argCol, const vector<int> &argInd): columns(argCol), indirection(argInd) {}
    Table operator[](Slice slice);//Negative indices count from the end of the array. Negative indices are resolved when the subscript operator is called, such that if you create a view with a index -1 and then push a row, it then points to the second to last item. !!!Modifies slice!!!
    Table operator[](int index) {this->operator[](Slice(index));}
    Table operator[](string name); //grab columns. Use semicolons to seperate multiple names. Due to an unfortunate implementation detail, this linear and not constant time, and someTable[index][name].get() is faster than someTable[name][index].get()
    template <class T> T& get(); //if *this contains a single element of type T, return it, otherwise throw an error
    void merge(Table& other, string name);
    //For these operators, argument must be a row or column vector, or the same siz If its a col vec, its applied to all columns. If its a row vec, its applied to all rows.
	template <class T> Table& operator+=(const Table& rhs);
	template <class T> Table operator+(const Table& rhs) {return this->deepCopy<T>()<T>+=rhs;}
	/*template <class T> Table& operator-=(const Table& rhs);
	template <class T> Table operator-(const Table& rhs) {return this->deepCopy<T>()<T>-=rhs;}
	template <class T> Table& operator*=(const Table& rhs);
	template <class T> Table operator*(const Table& rhs) {return this->deepCopy<T>()<T>*=rhs;}
	template <class T> Table& operator/=(const Table& rhs);
	template <class T> Table operator/(const Table& rhs) {return this->deepCopy<T>()<T>/=rhs;}*/ //wait till addition works to copy and paste
    template <class T> Table& sumCols(); //sum all columns. Due to implementation details, all columns must be the same type, and you have to tell us what it is.
    template <class T> Table& sumRows(); //sum all rows. Due to implementation details, all columns must be the same type, and you have to tell us what it is.
    // FIXME: summing over rows/columns meeting a certain condition would be nice
};

#include <Table.cpp>

