#include <user.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <antenna.h>
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
    try {
            file.open(fname);
    }
    catch (...) {
        cout << "Could not open file " << fname << ". Skipping file." << endl;
    }
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

vector<valarray<int>> parseEvents(int argc, char *argv[], int maxEvents) {
    int counter=0;
    ifstream file;
    string line;
    bool breaking=false;
    vector<valarray<int>> events;
    
    for(int i=2; i<argc; ++i) {
        try {
            file.open(argv[i]);
        }
        catch (...) {
            cout << "Could not open file " << __argv[i] << ". Skipping file." << endl;
        }
        while(file.good()) {
            getline(file,line);
            valarray<int> splitLine;
            try {
                splitLine=splitConvert<int>(line,"\t :-");
            }
            catch (...) {
                cout << "Failed to parse a line of " << __argv[i] << ". Continuing." << endl;
                continue;
            }
            if(splitLine[EV_ANTENNA]!=-1)
                events.push_back(splitLine);
            if(++counter>=maxEvents) {
                breaking=true;
                break;
            }
        }
        file.close();
        if(breaking)
            break;
    }
    return events;
}        
    
int main(int __argc, char* __argv[]) {
    //first argument is antenna table, all other arguments list event tables
    //print first 100 events and their locations
    assert(__argc>=3);
    string line;
    int nEvents;
    while(true) {
        cout << "Type a number of events to process or 'quit'." << endl;
        cin >> line;
        if(line==string("quit"))
            return 0;
        try {
            nEvents=lexical_cast<int>(line);
        }
        catch (...) {
            cout << "Command unrecognized." << endl;
            continue;
        }

        multiDimVala<float> antennas(parseAntennas(__argv[1]));
        multiDimVala<int> events(parseEvents(__argc, __argv, nEvents));
        //The data wasn't formatted by a programmer because it starts counting from 1.
        events.getView(1,EV_ANTENNA)-=valarray<int>(1,nEvents);
        events.getView(1,EV_UID)-=valarray<int>(1,nEvents);
        User::antennas=antennas;
        vector<User> users;
        for(int i=0; i<nEvents; ++i) {
            valarray<int> cur=events.getCopy(0,i);
            int uid=cur[EV_UID];
            if(uid >= users.size())
                users.push_back(User());
            users[uid].addEvent(cur);
        }
    
        int nChanged=0;
        for(int i=0; i<users.size(); ++i) {
            multiDimVala<float> smoothed=users[i].getSmoothed();
            multiDimVala<float> original=users[i].getOriginal();
            for(int j=0; j<smoothed.shape[0]; ++j)
                if(Antenna(&smoothed,j).nearest(antennas)!=Antenna(&original,j))
                    ++nChanged;
        }

        cout << nChanged << " out of " << nEvents << " were changed." << endl;
        cout << float(nChanged)/float(nEvents) << " were changed." << endl;
    }
}
