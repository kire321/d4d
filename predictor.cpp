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

int read_event(Event* event, istream& file)
{
    string line;
    getline(file,line);
    valarray<int> splitLine;
    try {
        splitLine=splitConvert<int>(line,"\t :-");
        if (splitLine[EV_UID] == CLEAR_USERS) {
            return CLEAR_USERS;
        }
    } catch (...) {
        cerr << "Failed to parse a line of events\n";
        return ERR;
    }
    if (splitLine[EV_ANTENNA]!=-1) {
        User::to_event(splitLine, event);
        if (LOG) cerr << "Converted to event";
    }
    return splitLine[EV_ANTENNA];
}

void parse_events(istream& file)
{
    Event event;
    int status;
    while (file.good()) {
        status = read_event(&event, file);
        if (status == ERR) {
            continue;
        } else if (status == CLEAR_USERS) {
            UserModel::clear();
            continue;
        }
        if (LOG) cerr << "Read event\n";

        UserModel::add_user(event.user_id);
        User* user = UserModel::find_user_by_id(event.user_id);
        if (LOG) cerr << "got user for event\n";

        // Get endpoints of path that would cover the new event so we can make
        // a prediction based on previous data
        Event likely_event, previous_event;
        user->next_likely_event(&event, &likely_event);
        if (LOG) cerr << "Got next likely location for user\n";
        user->previous_event(&previous_event);
        if (LOG) cerr << "Got previous location for user\n";

        if (previous_event.antenna_id > 0) { // -1 means user has no history
            assert(likely_event.antenna_id > 0);

            Event prediction;
            prediction.user_id = user->get_id();
            prediction.time = event.time;
            prediction.antenna_id = previous_event.antenna_id;
            // No motion if delta < 1 timestep
            int delta_time = User::to_minutes(event.time -
                previous_event.time);
            if (delta_time > AntennaModel::timestep) {
                int num_steps = delta_time / AntennaModel::timestep;
                int path_len = User::to_minutes(likely_event.time -
                    previous_event.time) / AntennaModel::timestep;

                AntennaId prediction_location = AntennaModel::predict_location(
                    previous_event.antenna_id, likely_event.antenna_id,
                    num_steps, path_len);
                if (LOG) cerr << "predicted a location\n";

                prediction.antenna_id = prediction_location;
            }
            cout << User::to_json(&prediction, true) << endl;
            if (LOG) cerr << "Made a prediction\n";
        }

        UserModel::update(&event);
        cout << User::to_json(&event, false) << endl;
        if (LOG) cerr << "Updated user model\n";
        AntennaModel::update(&event);
        if (LOG) cerr << "Updated Antenna model\n";
    }
    if (LOG) cerr << "Done parsing events\n";
}

int main(int argc, char** argv)
{
    // Initialize Antenna Model
    char* antenna_filename = argv[1];
    int timestep = atoi(argv[2]);
    ifstream antenna_file;
    string line;
    try {
        antenna_file.open(antenna_filename);
    } catch (...) {
        cerr << "Could not open file " << antenna_filename <<
          ". Skipping file." << endl;
    }
    AntennaModel::init(antenna_file, timestep);
    antenna_file.close();

    UserModel::init();

    srand(time(NULL));

    // Read in events and make predictions
    parse_events(cin);
    if (LOG) cerr << "Parsed events\n";

    return EXIT_SUCCESS;
}
