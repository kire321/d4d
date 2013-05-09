#include "antenna_model.h"
#include "utils.h"
#include "user_model.h"

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;

map<AntennaId, vector<AntennaId> > AntennaModel::transitions = map<AntennaId,
  vector<AntennaId> >();
map<AntennaId, Antenna*> AntennaModel::antennas = map<AntennaId, Antenna*>();

void AntennaModel::init(ifstream& antenna_file, ifstream& training_data_file)
{
    string line;
    while (antenna_file.good()) {
        getline(antenna_file, line);
        if (LOG) cerr << line << std::endl;
        try {
            valarray<float> antenna_data = splitConvert<float>(line, "\t");
            add_antenna(antenna_data);
        } catch (...) {
            cerr << "Failed to parse a line in antenna file. Continuing." << endl;
        }
        if (LOG) cerr << "Added antenna\n";
    }

    while (training_data_file.good()) {
        try {
            // FIXME: Come up with a format for this. Maybe just i, j -> list?
        } catch (...) {
            cerr << "Failed to line of training data. Continuing\n";
        }
    }
    if (LOG) cerr << "Trained model\n";
}

bool AntennaModel::add_antenna(valarray<float> antenna_data)
{
    AntennaId id = (AntennaId)antenna_data[0];
    float lat = antenna_data[1];
    float lon = antenna_data[2];

    if (LOG) cerr << "Adding antenna: " << id << endl;
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

    // FIXME: this is kind of funny with interpolation
    int num_steps = user->estimate_num_steps(last_event->time, time);
    Path interpolated_path = Path::interpolate_path(origin_antenna_id,
        antenna_id, num_steps);
    if (LOG) std::cerr << "Interpolated path\n";

    AntennaId current_antenna, next_antenna;
    current_antenna = interpolated_path.get_next_step(true);
    while ((next_antenna = interpolated_path.get_next_step()) != -1) {
        // We only care about bona fide transitions
        if (current_antenna != next_antenna) {
            transitions[current_antenna].push_back(next_antenna);
            current_antenna = next_antenna;
        }
    }
}

Antenna* AntennaModel::find_antenna_by_id(AntennaId id)
{
    if (LOG) cerr << "Finding id: " << id << endl;
    if (antennas.find(id) != antennas.end()) {
        return antennas[id];
    } else {
        return NULL;
    }
}

Antenna* AntennaModel::find_nearest_antenna(float lat, float lon)
{
    // TODO: use a P-R quadtree?
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

AntennaId AntennaModel::next_step_prediction(AntennaId start,
    time_duration time)
{
    if (LOG) std::cerr << "making next step pred\n";

    // Vector of all transitions made
    vector<AntennaId> next_steps = transitions[start];
    if (next_steps.size() != 0) {
        // TODO: if size is greater than INT_MAX, not random
        return next_steps.at(rand() % next_steps.size());
    } else {
        // If we have no history, return the the same location
        return start;
    }
}
