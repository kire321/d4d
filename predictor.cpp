#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <string>

#include "globals.h"
#include "antenna_model.h"
#include "user_model.h"

using namespace std;

int main(int argc, char** argv)
{
    // Initialize Antenna Model
    char* antenna_filename = argv[1];
    ifstream antenna_file;
    string line;
    try {
        antenna_file.open(antenna_filename);
    } catch (...) {
        cout << "Could not open file " << antenna_filename <<
          ". Skipping file." << endl;
    }
    g_antenna_model = AntennaModel(antenna_file);
    antenna_file.close();

    // Initialize new User Model
    g_user_model = UserModel();

    // Open file with events
    char* event_filename = argv[2];
    ifstream event_file;
    try {
        event_file.open(event_filename);
    } catch (...) {
        cout << "Could not open file " << event_filename <<
          ". Skipping file.\n";
    }
    // Read in events and make predictions
    parse_events(event_file, g_antenna_model, g_user_model);
    event_file.close();

    ofstream statistics_file;
    try {
        statistics_file.open("stats.txt", "w");
    } catch (...) {
        cout << "Could not open stats file for writing.\n";
    }
    antenna_model.print_statistics(statistics_file);
    user_model.print_statistics(statistics_file);

    return EXIT_SUCCESS;
}

void parse_events(ifstream& file, AntennaModel& antenna_model, UserModel& user_model)
{
    Event event; // FIXME: type for table
    User user;
    while (file.good()) {
        read_event(&event, file);
        user = user_model.update(&event);
        event = antenna_model.update(&event);

        Path predicted_path = antenna_model.path_prediction(event->antenna_id,
            user->next_likely_location());
        Path predicted_path_no_endpoint = antenna_model.path_prediction(
            event->antenna_id);
        // FIXME: also add non-endpoint prediction
        user->add_prediction(predicted_path);
    }
}
