#include <iostream>

#include "antenna_model.h"
#include "utils.h"
#include "user_model.h"
#include "user.h"

using std::cout;
using std::cerr;
using std::endl;

map<AntennaId, map<AntennaId, map<unsigned, vector<AntennaId> > > >
    AntennaModel::transitions =
    map<AntennaId, map<AntennaId, map <unsigned, vector<AntennaId> > > >();
map<AntennaId, Antenna*> AntennaModel::antennas =
    map<AntennaId, Antenna*>();
int AntennaModel::timestep = 0;

void AntennaModel::init(ifstream& file, int step)
{
    string line;
    while (file.good()) {
        getline(file, line);
        if (LOG) std::cout << line << std::endl;
        try {
            valarray<float> antenna_data = splitConvert<float>(line, "\t");
            add_antenna(antenna_data);
        } catch (...) {
            cout << "Failed to parse a line in antenna file. Continuing." << endl;
        }
        if (LOG) std::cout << "Added antenna\n";
    }

    timestep = step;
}

bool AntennaModel::add_antenna(valarray<float> antenna_data)
{
    AntennaId id = (AntennaId)antenna_data[0];
    float lat = antenna_data[1];
    float lon = antenna_data[2];

    if (LOG) cout << "Adding antenna: " << id << endl;
    if (!find_antenna_by_id(id)) {
        antennas[id] = new Antenna(lat, lon, id);
        return true;
    }
    return false;
}

void AntennaModel::update(Event* event)
{
    UserId user_id = event->user_id;
    AntennaId antenna_id = event->antenna_id;
    ptime time = event->time;

    User* user = UserModel::find_user_by_id(user_id);
    Event* last_event = user->get_last_event();
    AntennaId origin_antenna_id = last_event->antenna_id;
    if (LOG) std::cerr << "Found last user event. Interpolating path\n";

    int elapsed_minutes = User::to_minutes(time - last_event->time);
    int num_steps = elapsed_minutes / timestep;
    Path interpolated_path = Path::interpolate_path(origin_antenna_id,
        antenna_id, num_steps);
    if (LOG) std::cerr << "Interpolated path\n";

    // Increment next-step transition frequency for the i,j path on t time units
    AntennaId current_antenna, next_antenna;
    current_antenna = interpolated_path.get_next_step(true);
    while ((next_antenna = interpolated_path.get_next_step()) != -1) {
        transitions[current_antenna][interpolated_path.get_last_step()][
            num_steps--].push_back(next_antenna);
        current_antenna = next_antenna;
    }
}

Antenna* AntennaModel::find_antenna_by_id(AntennaId id)
{
    if (LOG) cout << "Finding id: " << id << endl;
    if (antennas.find(id) != antennas.end()) {
        return antennas[id];
    } else {
        return NULL;
    }
}

Antenna* AntennaModel::find_nearest_antenna(float lat, float lon)
{
    // FIXME: make more efficient -- currently linear
    Antenna* nearest = antennas.begin()->second;
    map<AntennaId, Antenna*>::iterator ant;
    for (ant = antennas.begin(); ant != antennas.end(); ant++) {
        Antenna* antenna = ant->second;
        if (antenna->distance_from(lat, lon) <
            nearest->distance_from(lat, lon)) {
            nearest = antenna;
        }
    }
    return nearest;
}

AntennaId AntennaModel::predict_location(AntennaId start, AntennaId end,
    unsigned num_steps, unsigned path_len)
{
    if (LOG) std::cerr << "making path prediction\n";

    assert(num_steps <= path_len && num_steps > 0);

    AntennaId next_step = start;
    for (; num_steps > 0; num_steps--) {
        next_step = next_step_prediction(next_step, end, path_len--);
    }

    return next_step;
}

AntennaId AntennaModel::next_step_prediction(AntennaId start, AntennaId end,
    unsigned num_steps)
{
    if (LOG) std::cerr << "making next step pred\n";

    // Vector of all transitions made
    vector<AntennaId> next_steps = transitions[start][end][num_steps];
    if (next_steps.size() != 0) {
        return next_steps.at(rand() % next_steps.size());
    } else {
        // If we have no history, return the first step along the straight line
        // path (normalized for time)
        Path straight_line_path = Path::interpolate_path(start, end, num_steps);
        straight_line_path.get_next_step(true);
        return straight_line_path.get_next_step();
    }
}
