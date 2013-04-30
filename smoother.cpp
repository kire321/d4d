#include <assert.h>
#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <fstream>
#include <string>

#include "types.h"
#include "utils.h"
#include "antenna_model.h"
#include "user_model.h"

using namespace std;

int num_events = 0;

int read_event(Event* event, ifstream& file)
{
    string line;
    getline(file,line);
    valarray<int> splitLine;
    try {
        splitLine=splitConvert<int>(line,"\t :-");
    } catch (...) {
        cerr << "Failed to parse a line of events\n";
        return -1;
    }
    if (splitLine[EV_ANTENNA]!=-1) {
        User::to_event(splitLine, event);
        if (event->user_id == 21319 && event->hour == 8 && event->day == 11203 && event->antenna_id == 240) {
          cerr << "hit this event\n";
        }
        if (LOG) cerr << "Converted to event\n";
    }
    return splitLine[EV_ANTENNA];
}

void parse_events(ifstream& file)
{
    Event event;
    while (file.good()) {
        if (read_event(&event, file) < 0) continue;
        UserModel::update(&event);
        num_events++;
    }
}

int main(int argc, char** argv)
{
    // Initialize Antenna Model
    assert(argc > 2);
    char* antenna_filename = argv[1];
    ifstream antenna_file;
    string line;
    try {
        antenna_file.open(antenna_filename);
    } catch (...) {
        cerr << "Could not open file " << antenna_filename <<
          ". Skipping file." << endl;
    }
    AntennaModel::init(antenna_file);
    antenna_file.close();

    UserModel::init();

    // Open file with events
    char* event_filename = argv[2];
    ifstream event_file;
    try {
        event_file.open(event_filename);
    } catch (...) {
        cerr << "Could not open file " << event_filename <<
          ". Skipping file.\n";
    }
    // Read in events and make predictions
    parse_events(event_file);

    if (LOG) cerr << "Parsed events\n";
    event_file.close();

    // Compute changed under smoothing
    int num_changed = 0;
    map<UserId, User*>::iterator user;
    for (user = UserModel::users.begin(); user != UserModel::users.end();
        user++) {
        multiDimVala<float> smoothed = (user->second)->getSmoothed();
        vector<Event*> events = (user->second)->get_events();

        for (int j = 0; j < smoothed.shape[0]; j++) {
            valarray<float> smoothed_loc = smoothed.getView(0, j);
            Antenna* nearest = AntennaModel::find_nearest_antenna(smoothed_loc[0],
                smoothed_loc[1]);
            if (nearest->get_id() != events[j]->antenna_id) {
                num_changed++;
            }
        }

    }

    cout << num_changed << " out of " << num_events << " were changed." << endl;
    cout << float(num_changed)/float(num_events) << " were changed." << endl;

    return EXIT_SUCCESS;
}
