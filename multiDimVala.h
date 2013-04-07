#include <valarray>
#include <vector>
#include <assert.h>
#pragma once
using namespace std;

template <class T>
class multiDimVala {
	//The rows/columns returned by this class are not views, the data is deepcopied.
	//TODO: make that comment untrue!
	//Sorry, you can only make two-dimensional valas
public:
	valarray<T> data;
	vector<int> shape;
	//The vector spans dimension 0
	multiDimVala() {}
	multiDimVala(int d0, int d1): data(valarray<T>(d0*d1)) {
		shape.push_back(d0);
		shape.push_back(d1);
	}
	multiDimVala(vector<valarray<T>> vvT);
	slice_array<T> getView(int dimension, int index); //returns views. Changes to views are reflected in original, but views are not subscriptable. Faster, since no information is copied
	valarray<T> getCopy(int dimension, int index) {return valarray<T>(getView(dimension,index));} // returns a deepcopy of the original row/column. Subscriptable, but modifications to the deepcopy do not effect the original. Slower, since the data is actually copied
	T& getSingle(int i0, int i1); //returns a single item from the array. Changes to this item are reflected in the original.
	valarray<T> sum(int dim); //STL is annoying and won't let us sum over views
};

#include <multiDimVala.cpp>
