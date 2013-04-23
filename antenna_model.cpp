#include <random>

#include "antenna_model.h"
#include "utils.h"
#include "user_model.h"
#include "user.h"

vector<Antenna*>* AntennaModel::antennas = NULL;
int**** AntennaModel::transition_frequencies = NULL;
map<AntennaId, unsigned> AntennaModel::antenna_id_map =
    map<AntennaId, unsigned>();

void AntennaModel::init(ifstream& file)
{
    antennas = new vector<Antenna*>();
    transition_frequencies = (int****)calloc(2048, 2048 * 24 * 2048 * sizeof(int));

    // Initialize distribution to be uniform (no, there's no easier way to do
    // this)
    for (int i = 0; i < 2048; i++) {
        for (int j = 0; j < 2048; j++) {
            for (int k = 0; k < 24; k++) {
                for (int l = 0; l < 2048; l++) {
                    transition_frequencies[i][j][k][l] = 1;
                }
            }
        }
    }

    string line;
    while (file.good()) {
        getline(file, line);
        try {
            valarray<float> antenna_data = splitConvert<float>(line, "\t")[slice(1,2,1)];
            add_antenna(antenna_data);
        }
        catch (...) {
            cout << "Failed to parse a line in antenna file. Continuing." << endl;
        }
    }
}

bool AntennaModel::add_antenna(valarray<float> antenna_data)
{
    AntennaId id = (AntennaId)antenna_data[0];
    float lat = antenna_data[1];
    float lon = antenna_data[2];

    if (!find_antenna_by_id(id)) {
        antennas->push_back(new Antenna(lat, lon, id));
        antenna_id_map[id] = antennas->size() - 1;
        // if (id >= transition_frequencies->size()) {
        //     transition_frequencies->resize(2 * transition_frequencies->size());
        // }
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

    Path interpolated_path = Path::interpolate_path(origin_antenna_id,
        antenna_id, hour);

    // Increment next-step transition frequency for the i,j path on t time units
    AntennaId current_antenna, next_antenna;
    current_antenna = interpolated_path.get_next_step(true);
    while ((next_antenna = interpolated_path.get_next_step()) !=
        interpolated_path.get_last_step()) {
        transition_frequencies[current_antenna][interpolated_path.get_last_step()][elapsed_time--][interpolated_path.get_first_step()]++;
    }
}

Antenna* AntennaModel::find_antenna_by_id(AntennaId id)
{
    return antennas->at(antenna_id_map[id]);
}

Antenna* AntennaModel::find_nearest_antenna(float lat, float lon)
{
    // TODO: make more efficient
    Antenna* nearest = *(antennas->begin());
    vector<Antenna*>::iterator ant;
    for (ant = antennas->begin() + 1; ant != antennas->end(); ant++) {
        if ((*ant)->distance_from(lat, lon) <
            nearest->distance_from(lat, lon)) {
            nearest = *ant;
        }
    }
    return nearest;
}

Path AntennaModel::path_prediction(AntennaId start, AntennaId end,
    unsigned time)
{
    Path predicted_path;
    predicted_path.add_step(start);

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
    predicted_path.add_step(start);

    for (; time > 0; time--) {
        predicted_path.add_step(next_step_prediction(
            predicted_path.get_last_step(), time));
    }

    return predicted_path;
}

AntennaId AntennaModel::next_step_prediction(AntennaId start, AntennaId end, unsigned time)
{
    std::default_random_engine generator;

    vector<unsigned> frequencies(&transition_frequencies[start][end][time][0],
        &transition_frequencies[start][end][time][antennas->size()]);
    std::discrete_distribution<int> distribution(frequencies.begin(),
        frequencies.end());

    return distribution(generator);
}

AntennaId AntennaModel::next_step_prediction(AntennaId current, unsigned time)
{
    std::default_random_engine generator;

    unsigned num_antennas = antennas->size();
    vector<unsigned> frequencies(num_antennas);
    for (unsigned i = 0; i < num_antennas; i++) {
        unsigned num_paths = 0;
        for (unsigned j = 0; j < num_antennas; j++) {
            for(unsigned k = 0; k < 24; k++) {
                num_paths += transition_frequencies[current][j][k][i]; // TODO: slow as heck
            }
        }
        frequencies[i] = num_paths;
    }
    // FIXME: will choke if distribution is all 0s
    std::discrete_distribution<int> distribution(frequencies.begin(),
        frequencies.end());

    return distribution(generator);
}
