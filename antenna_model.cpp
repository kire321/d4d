#include <random>

#include "antenna_model.h"
#include "user.h"

void AntennaModel::init()
{
    antennas = new vector();
    antenna_id_map = new map();
    transition_frequencies = new valarray<valarray<valarray<valarray<unsigned> > > >(2048, 0);
}

AntennaModel::AntennaModel()
{
    init()
}

AntennaModel::AntennaModel(ifstream& file)
{
    init();

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

AntennaModel::~AntennaModel()
{
    delete antennas;
    delete antenna_id_map;
    delete transition_frequencies;
}

bool AntennaModel::add_antenna(valarray<float> antenna_data)
{
    AntennaId id = (AntennaId)antenna_data[0];
    float lat = antenna_data[1];
    float lon = antenna_data[2];

    if (!find_antenna_by_id(id)) {
        antennas.push_back(pair<float, float>(lat, lon));
        antenna_id_map[id] = antennas.size() - 1;
        if (id >= transition_frequencies.size()) {
            transition_frequencies.resize(2 * transition_frequencies.size());
        }
        return true;
    }
    return false;
}

void AntennaModel::update(Event* event)
{
    UserId user_id = event->user_id;
    AntennaId antenna_id = event->antenna_id;
    unsigned time = event->time;

    User* user = UserModel->find_user_by_id(uid); // FIXME
    Event* last_event = user->get_last_event();
    AntennaId origin_antenna_id = last_event->antenna_id;
    unsigned elapsed_time = time - last_event->time;

    Path interpolated_path = Path::interpolate(origin_antenna_id, antenna_id,
        time);

    // Increment next-step transition frequency for the i,j path on t time units
    AntennaId current_antenna, next_antenna;
    current_antenna = path.get_next_step(true);
    while (next_antenna = path.get_next_step() != path.last_step()) {
        transition_frequencies[current_antenna][path.last_step()][elapsed_time--][path.get_first_step()]++;
    }
}

Antenna* AntennaModel::find_by_id(AntennaId id)
{
    return antennas[antenna_id_map[id]];
}

Antenna* AntennaModel::find_nearest_antenna(float lat, float lon)
{
    // TODO: make more efficient
    AntennaId nearest = (*antennas.begin())->get_id();
    vector<Antenna*>::iterator ant;
    for (ant = antennas.begin() + 1; ant != antennas.end(); ant++) {
        if ((*ant)->distance_from(lat, lon) <
            nearest.distance_from(lat, lon)) {
            nearest = (*ant)->get_id();
        }
    }
}

Path AntennaModel::path_prediction(AntennaId start, AntennaId end, unsigned time);
{
    Path predicted_path;
    predicted_path.add_step(start);

    for (; elapsed_time > 1; elapsed_time--) {
        predicted_path.add_step(next_step_prediction(path.get_last_step(), end,
            elapsed_time);
    }

    predicted_path.add_step(end);
}

Path AntennaModel::path_prediction(AntennaId start, unsigned time);
{
    Path predicted_path;
    predicted_path.add_step(start);

    for (; elapsed_time > 1; elapsed_time--) {
        predicted_path.add_step(next_step_prediction(path.get_last_step(),
            elapsed_time);
    }

    predicted_path.add_step(end);
}

AntennaId AntennaModel::next_step_prediction(AntennaId start, AntennaId end, unsigned time)
{
    std::default_random_engine generator;

    std::vector<unsigned> frequencies =
        transition_frequencies[start][end][time];
    // FIXME: will choke if distribution is all 0s
    std::discrete_distribution<int> distribution(frequencies.begin(),
        frequencies.end());

    return distribution(generator);
}

AntennaId AntennaModel::next_step_prediction(AntennaId current, unsigned time)
{
    std::default_random_engine generator;

    std::valarray<unsigned> frequencies(num_antennas);
    unsigned num_antennas = antennas.size();
    for (int i = 0; i < num_antennas; i++) {
        unsigned num_paths = 0;
        for (int j = 0; j < num_antennas; j++) {
            num_paths += transition_frequencies[i][j][time];
        }
        frequences[i] = num_paths;
    }
    // FIXME: will choke if distribution is all 0s
    std::discrete_distribution<int> distribution(frequencies.begin(),
        frequencies.end());

    return distribution(generator);
}
