//This include makes it easier to edit this file with VS
#include <multiDimVala.h>

template <class T>
int argmin(valarray<T> vala) {
    assert(vala.size()>=1);
    int minIndex=0;
    for(int i=0; i<vala.size(); ++i)
        if(vala[i]<vala[minIndex])
            minIndex=i;
    return minIndex;
}

template <class T>
multiDimVala<T>::multiDimVala(vector<valarray<T>> vvT) {
    shape=vector<int>(2);
    shape[0]=vvT.size();
    shape[1]=vvT[0].size();
    data=valarray<T>(shape[0]*shape[1]);
    for(int i=0; i<vvT.size(); ++i)
        data[slice(i*vvT[0].size(),vvT[0].size(),1)]=vvT[i];
}

template <class T>
slice_array<T> multiDimVala<T>::getView(int dimension, int index) {
    assert(dimension>=0 && dimension<=1);
    assert(index>=0 && index<shape[dimension]);
    int start,step;
    if(dimension==0) {
        start=index*shape[1];
        step=1;
    }
    else {
        start=index;
        step=shape[1];
    }
    return data[slice(start,shape[1-dimension],step)];
}

template <class T>
T multiDimVala<T>::getSingle(int d0, int d1) {
    assert(0<=d0 && d0<shape[0]);
    assert(0<=d1 && d1<shape[1]);
    return data[shape[1]*d0+d1];
}

template <class T>
valarray<T> multiDimVala<T>::sum(int dim) {
    assert(dim>=0 && dim<=1);
    valarray<T> toret(shape[1-dim]);
    for(int i=0; i<toret.size(); ++i) {
        T total=0;
        for(int j=0; j<shape[dim]; ++j) {
            if(dim==0)
                total+=getSingle(j,i);
            else
                total+=getSingle(i,j);
        }
        toret[i]=total;
    }
    return toret;
}

template <class T>
multiDimVala<T>& multiDimVala<T>::operator+=(const valarray<T>& rhs) {
    assert(rhs.size()==shape[1]);
    for(int i=0; i<shape[0]; ++i)
        getView(0,i)+=rhs;
    return *this;
}

template <class T>
multiDimVala<T>& multiDimVala<T>::operator*=(const valarray<T>& rhs) {
    assert(rhs.size()==shape[1]);
    for(int i=0; i<shape[0]; ++i)
        getView(0,i)*=rhs;
    return *this;
}

template <class T>
multiDimVala<T>& multiDimVala<T>::operator/=(const valarray<T>& rhs) {
    assert(rhs.size()==shape[1]);
    for(int i=0; i<shape[0]; ++i)
        getView(0,i)/=rhs;
    return *this;
}
