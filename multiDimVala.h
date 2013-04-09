#include <valarray>
#include <vector>
#include <assert.h>
#include <iostream>
#pragma once
using namespace std;

//Sadly, I'm not joking. STL doesn't have a built-in way to find the min or max index in a valarray.
template <class T> int argmin(valarray<T> vala);
template <class T> int argmax(valarray<T> vala) {return argmin(vala*-1);}

template <class T>
class multiDimVala {
    //Sorry, you can only make two-dimensional valas
public:
    valarray<T> data;
    vector<int> shape;
    multiDimVala() {}
    multiDimVala(const multiDimVala& other): data(other.data), shape(other.shape) {}
    multiDimVala(int d0, int d1): data(valarray<T>(d0*d1)) {
        shape.push_back(d0);
        shape.push_back(d1);
    }
    multiDimVala(vector<valarray<T>> vvT);
    slice_array<T> getView(int dimension, int index); //returns views. Changes to views are reflected in original, but views are not subscriptable. Faster, since no information is copied
    valarray<T> getCopy(int dimension, int index) {return valarray<T>(getView(dimension,index));} // returns a deepcopy of the original row/column. Subscriptable, but modifications to the deepcopy do not effect the original. Slower, since the data is actually copied
    T getSingle(int i0, int i1); //returns a single item from the array. Changes to this item are reflected in the original.
    valarray<T> sum(int dim); //STL is annoying and won't let us sum over views
    multiDimVala<T>& operator+=(const valarray<T>& rhs); //Arithmatic requires you match the innermost dimension
    multiDimVala<T> operator+(const valarray<T>& rhs) {return multiDimVala<T>(*this)+=rhs;}
    multiDimVala<T>& operator-=(const valarray<T>& rhs) {return multiDimVala<T>(*this)+=-rhs;}
    multiDimVala<T> operator-(const valarray<T>& rhs) {return *this+-rhs;}
    multiDimVala<T>& operator*=(const valarray<T>& rhs);
    multiDimVala<T>& operator*(const valarray<T>& rhs) {return multiDimVala<T>(*this)*=rhs;}
    multiDimVala<T>& operator/=(const valarray<T>& rhs);
    multiDimVala<T>& operator/(const valarray<T>& rhs) {return multiDimVala<T>(*this)/=rhs;}
};

#include <multiDimVala.cpp>
