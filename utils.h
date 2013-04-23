#pragma once

#include <valarray>
#include <vector>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using std::vector;
using std::valarray;
using std::string; // FIXME

template <class T>
valarray<T> splitConvert(string line, const char *seperator)
{
    vector<string> splitVec;
    split(splitVec,line,is_any_of(seperator),token_compress_on);
    valarray<T> toret(splitVec.size());
    for(unsigned i=0; i<splitVec.size(); ++i) {
        toret[i]=lexical_cast<T>(splitVec[i]);
    }
    return toret;
}

