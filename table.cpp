#include "Table.h"
#pragma once
#ifndef TABLE
#define TABLE

template <class T> void Column::setHash() {
    assert(typeHash==0);
    typeHash=typeid(T).hash();
}

void Table::addRow() {
    for(colmap::iterator it=columns.begin(); it!=columns.end(); ++it) {
        Column& col=it->second;
        int i;
        int nextIndex=-1;
        if(col.thisTree) {
            i=0;
            col.data->emplace_back();
            nextIndex=col.data->size()-1;
        } else
            i=1;
        assert(nextIndex>=0);
        for(; i<col.indirections.size(); ++i) {
            vector<int>& indir=*col.indirections[i];
            if(*indir.end() != nextIndex)
                indir.push_back(nextIndex);
            nextIndex=indir.size()-1;
        }
    }
}

int Table::size() const {
    const Column& firstCol=columns.begin()->second;
    if(firstCol.indirections.size()==0) //In this case we are the root
        return firstCol.data->size();
    else
        return firstCol.indirections[firstCol.indirections.size()-1]->size();
}

template <class T>
void Table::addCol(string name) {
    Column newCol;
    newCol.setType<T>();
    newCol.data=new vector<any>(size());
    newCol.thisTree=true;
    columns.insert(pair<string,Column>(name,newCol));
}

Table::Table(const Table& other) {//we must deepcopy stack variables plus any data owned by other
    columns=other.columns;
    indirection=other.indirection;
    for(colmap::iterator it=columns.begin(); it!=columns.end(); ++it)
        if(it->second.indirections.size()==0)
            it->second.data=new vector<any>(*it->second.data);
}

Table Table::operator[](Slice slice) {
    int psize=size();
    if(slice.begin<0)
        slice.begin+=psize;
    if(slice.end<0)
        slice.end+=psize;
    assert(slice.begin>=0 && slice.begin<psize);
    assert(slice.end>=0 && slice.end<psize);
    vector<int> newIndir;
    for(int i=slice.begin; i<slice.end; i+=slice.step)
        newIndir.push_back(i);
    assert(newIndir.size()>=1);
    Table toret(columns,newIndir);
    for(colmap::iterator it=columns.begin(); it!=columns.end(); ++it)
        it->second.indirections.push_back(&toret.indirection);
    return toret;
}

Table Table::operator[](string name) {
    colmap newCols;
    vector<string> namesVec;
    split(namesVec,name,is_any_of(";"),token_compress_on);
    unordered_set<string> namesSet;
    for(int i=0; i<namesVec.size(); ++i)
        namesSet.insert(namesVec[i]);
    return *this;
    for(colmap::iterator it=columns.begin(); it!=columns.end(); ++it)
        if(namesSet.count(it->first)==1)
            newCols.insert(*it);
    assert(newCols.size()>0);
    vector<int> newIndir(size()); //Without this passthrough indirection, the view would think it managed the data, and deleting the view would break the original
    for(int i=0; i<newIndir.size(); ++i)        
        newIndir[i]=i;
    Table toret(newCols,newIndir);
    for(colmap::iterator it=toret.columns.begin(); it!=toret.columns.end(); ++it)
        it->second.indirections.push_back(&toret.indirection);
    return toret;
}

void Table::merge(Table &other, string name) {
    for(colmap::iterator it=other.columns.begin(); it!=other.columns.end(); ++it) {
        Column newCol=columns[name];
        newCol.data=it->second.data;
        vector<int> *newIndir=new vector<int>(size());
        for(int i=0; i<newIndir->size(); ++i)
            newIndir->at(i)=any_cast<int>(columns[name].data->at(i));
        newCol.indirections.push_back(newIndir);
        newCol.thisTree=false;
    }
}

template <class T> T& Table::get() {
    assert(size()==1);
    assert(columns.size()==1);
    assert(columns.begin()->second.typeHash==typeid(T).hash());
    int index=0;
    vector<vector<int> *> &indirs=columns.begin()->second.indirections
    for(int i=indirs.size()-1; i>=0; --i)
        index=indirs[i]->at(index);
    return any_cast<T>(columns.begin()->second.data[index]);
}

template <class T>
Table& Table::deepCopy() {
    int isize=size();
    colmap newMap;
    for(colmap::iterator it=columns.begin(); it!=columns.end(); ++it) {
        Column newCol;
        newCol.setType<T>();
        newCol.thisTree=true;
        newCol.data=new vector<any>(isize);
        newMap.insert(pair<string,Column>(it->first,newCol));
    }
    Table toret(newMap,vector<int>());
    for(int i=0; i<isize; ++i)
        for(colmap::iterator it=columns.begin(); it!=columns.end(); ++it)
            toret[i][it->first].get<T>()=T(operator[](i)[it->first].get<T>());
    return toret;
}

template <class T> Table& Table::operator+=(const Table& rhs) {
    int tsize=size();
    int rsize=rhs.size();
    if(rsize==1) {
        assert(columns.size()==rhs.columns.size()); //rhs is a single row, so apply rhs to all rows in this
        for(colmap::iterator it=columns.begin(); it!=columns.end(); ++it)
            for(int i=0; i<tsize; ++i)
                this->operator[](i)[it->first].get<T>()+=rhs[name].get<T>();
    } else if(rhs.columns.size()==1) {
        assert(tsize==rsize);
        for(colmap::iterator it=columns.begin(); it!=columns.end(); ++it)
            for(int i=0; i<tsize; ++i)
                this->operator[](i)[it->first].get<T>()+=rhs[i].get<T>();
    } else {
        assert(columns.size()==rhs.columns.size());
        assert(tsize==rsize);
        for(colmap::iterator it=columns.begin(); it!=columns.end(); ++it)
            for(int i=0; i<tsize; ++i)
                this->operator[](i)[it->first].get<T>()+=rhs[i][it->first].get<T>();
    }
}

template <class T> Table& Table::sumCols() {
    assert(columns.size()>=1);
    Table toret=this->operator[](columns.begin()->first).deepCopy();
    colmap::iterator it=columns.begin();
    ++it;
    for(; it!=columns.end(); ++it)
        toret<T>+=this->operator[](it->first);
    return toret;
}

template <class T> Table& Table::sumRows() {
    int tsize=size();
    assert(tsize>=1);
    Table toret=this->operator[](0).deepCopy();
    for(int i=1; i<tsize; ++i)
        toret<T>+=this->operator[](i);
    return toret;
}

#endif