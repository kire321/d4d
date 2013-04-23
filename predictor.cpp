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

void read_event(Event* event, ifstream& file)
{
    string line;
    getline(file,line);
    valarray<int> splitLine;
    try {
        splitLine=splitConvert<int>(line,"\t :-");
    } catch (...) {
        cout << "Failed to parse a line of events\n";
        return;
    }
    if (splitLine[EV_ANTENNA]!=-1) {
        User::to_event(splitLine, event);
    }
}

void parse_events(ifstream& file)
{
    Event event;
    while (file.good()) {
        read_event(&event, file);
        UserModel::update(&event);
        AntennaModel::update(&event);

        User* user = UserModel::find_user_by_id(event.user_id);
        AntennaId likely_location;
        unsigned likely_duration;
        user->next_likely_location(event.hour, &likely_location,
            &likely_duration);
        Path predicted_path = AntennaModel::path_prediction(event.antenna_id,
            likely_location, likely_duration);
        // Rerun with non-endpoint prediction
        // Path predicted_path_no_endpoint = antenna_model.path_prediction(
        //     event.antenna_id, event.hour);
        user->make_prediction(predicted_path, event.day, event.hour);
    }
}

int main(int argc, char** argv)
{
    // Initialize Antenna Model
    assert(argc > 2);
    char* antenna_filename = argv[0];
    ifstream antenna_file;
    string line;
    try {
        antenna_file.open(antenna_filename);
    } catch (...) {
        cout << "Could not open file " << antenna_filename <<
          ". Skipping file." << endl;
    }
    AntennaModel::init(antenna_file);
    antenna_file.close();

    UserModel::init();

    // Open file with events
    char* event_filename = argv[1];
    ifstream event_file;
    try {
        event_file.open(event_filename);
    } catch (...) {
        cout << "Could not open file " << event_filename <<
          ". Skipping file.\n";
    }
    // Read in events and make predictions
    parse_events(event_file);
    event_file.close();

    ofstream statistics_file;
    try {
        statistics_file.open("stats.txt", ios_base::out);
    } catch (...) {
        cout << "Could not open stats file for writing.\n";
    }
    // g_antenna_model.print_statistics(statistics_file);
    // g_user_model.print_statistics(statistics_file);

    return EXIT_SUCCESS;
}
