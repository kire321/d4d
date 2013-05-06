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
        UserModel::update(&event);
        if (LOG) cerr << "Updated user model\n";
        AntennaModel::update(&event);
        if (LOG) cerr << "Updated Antenna model\n";

        User* user = UserModel::find_user_by_id(event.user_id);
        AntennaId likely_location;
        unsigned likely_end;
        assert(user);
        if (LOG) cerr << "got user for event\n";
        user->next_likely_location(event.hour, &likely_end,
            &likely_location);
        if (LOG) cerr << "Got next likely location for user\n";
        int duration = (int)likely_end - (int)event.hour;
        if (duration <= 0) duration += 24;
        Path predicted_path = AntennaModel::path_prediction(event.antenna_id,
            likely_location, duration);
        if (LOG) cerr << "predicted a path\n";
        // Rerun with non-endpoint prediction
        // Path predicted_path_no_endpoint = antenna_model.path_prediction(
        //     event.antenna_id, event.hour);
        user->make_prediction(predicted_path, event.day, event.hour + 1);
        if (LOG) cerr << "Made a prediction\n";
    }
    if (LOG) cerr << "Done parsing events\n";
}

int main(int argc, char** argv)
{
    // Initialize Antenna Model
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

    srand(time(NULL));

    // Read in events and make predictions
    parse_events(cin);
    if (LOG) cerr << "Parsed events\n";

    return EXIT_SUCCESS;
}
