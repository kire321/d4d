#include <random>

#include "antenna_model.h"

AntennaModel::AntennaModel(ifstream& file)
{
    antennas = new Table(); // FIXME:

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

AntennaModel::AntennaModel()
{
    antennas = new Table();
}

AntennaModel::~AntennaModel()
{
    delete antennas;
}

bool AntennaModel::add_antenna(AntennaId id, double lat, double lon)
{
    if (!antennas->find_by_id(id)) {
        antennas.add_row(id, lat, lon);
        while (id >= transition_frequencies.size()) {
            antennas.add_row();
            antennas[antennas.size() - 1] = event;
            // FIXME: assignment here
            // TODO: add zero row
  // FIXME: dealing with antenna model id != index
  // "get_by_id"
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

    User* user = UserModel->find_user_by_id(uid);
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

Antenna* find_by_id(AntennaId id)
{
    return antennas->find_by_id(id);
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
