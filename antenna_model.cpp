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

void AntennaModel::init(ifstream& file)
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
    unsigned hour = event->hour;

    User* user = UserModel::find_user_by_id(user_id);
    Event* last_event = user->get_last_event();
    AntennaId origin_antenna_id = last_event->antenna_id;
    unsigned elapsed_time = hour - last_event->hour;
    if (elapsed_time < 0) elapsed_time += 24;
    if (LOG) std::cout << "Found last user event. Interpolating path\n";

    Path interpolated_path = Path::interpolate_path(origin_antenna_id,
        antenna_id, hour);
    if (LOG) std::cout << "Interpolated path\n";

    // Increment next-step transition frequency for the i,j path on t time units
    AntennaId current_antenna, next_antenna;
    current_antenna = interpolated_path.get_next_step(true);
    while ((next_antenna = interpolated_path.get_next_step()) != -1) {
        transitions[current_antenna][interpolated_path.get_last_step()][
            elapsed_time--].push_back(next_antenna);
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

Path AntennaModel::path_prediction(AntennaId start, AntennaId end,
    unsigned time)
{
    if (LOG) std::cerr << "making path prediction\n";

    assert(time > 0);

    Path predicted_path;
    predicted_path.add_step(next_step_prediction(start, end, time--));
    for (; time > 1; time--) {
        predicted_path.add_step(next_step_prediction(
            predicted_path.get_last_step(), end, time));
    }
    predicted_path.add_step(end);

    return predicted_path;
}

Path AntennaModel::path_prediction(AntennaId start, unsigned time)
{
    Path predicted_path;
    predicted_path.add_step(next_step_prediction(start, time--));
    for (; time > 0; time--) {
        predicted_path.add_step(next_step_prediction(
            predicted_path.get_last_step(), time));
    }

    return predicted_path;
}

AntennaId AntennaModel::next_step_prediction(AntennaId start, AntennaId end, unsigned time)
{
    if (LOG) std::cerr << "making next step pred\n";

    // Vector of all transitions made
    vector<AntennaId> next_steps = transitions[start][end][time];
    if (next_steps.size() != 0) {
        return next_steps.at(rand() % next_steps.size());
    } else {
        // If we have no history, return the first step along the straight line
        // path (normalized for time)
        Path straight_line_path = Path::interpolate_path(start, end, time);
        straight_line_path.get_next_step(true);
        return straight_line_path.get_next_step();
    }
}

AntennaId AntennaModel::next_step_prediction(AntennaId current, unsigned time)
{
    // TODO:
    return 1;
}
