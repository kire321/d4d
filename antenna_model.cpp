#include "antenna_model.h"

AntennaModel::AntennaModel()
{
    antennas = new multiDimVala<float>();
}

AntennaModel::~AntennaModel()
{
    delete antennas;
}

bool AntennaModel::add_antenna(unsigned id, double lat, double lon)
{
    if (!antennas->find_by_id(id)) {
        antennas.add_row(id, lat, lon);
    }
}

void AntennaModel::update(Event* event)
{
    unsigned user_id = event->user_id;
    unsigned antenna_id = event->antenna_id;
    unsigned time = event->time;

    User* user = UserModel->find_by_id(uid);
    Event* last_event = user->get_last_event();
    unsigned origin_antenna_id = last_event->antenna_id;
    unsigned elapsed_time = time - last_event->time;

    Path interpolated_path = Path::interpolate(origin_antenna_id, antenna_id,
        time);

    // Increment next-step transition frequency for the i,j path on t time units
    unsigned current_antenna, next_antenna;
    current_antenna = path.get_next_step(true);
    while (next_antenna = path.get_next_step() != path.last_step()) {
        transition_frequencies[current_antenna][path.last_step()][elapsed_time--][path.get_first_step()]++;
    }
}

Path AntennaModel::path_prediction(unsigned start, unsigned end, unsigned time);
{
    Path predicted_path;
    predicted_path.add_step(start);

    for (; elapsed_time > 1; elapsed_time--) {
        predicted_path.add_step(next_step_prediction(path.get_last_step(), end,
            elapsed_time);
    }

    predicted_path.add_step(end);
}

unsigned AntennaModel::next_step_prediction(unsigned start, unsigned end, unsigned time)
{
    // TODO: sample from vector of transition_frequencies[start][end][elapsed_time];
    return 0;
}
