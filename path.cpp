#include <assert.h>

#include "path.h"
#include "antenna_model.h"
#include "antenna.h"

Path Path::interpolate_path(AntennaId start, AntennaId end,
    unsigned num_steps)
{
    Antenna* start_antenna = AntennaModel::find_antenna_by_id(start);
    Antenna* end_antenna = AntennaModel::find_antenna_by_id(end);

    assert(start_antenna && end_antenna);

    float start_lat = start_antenna->get_latitude();
    float start_lon = start_antenna->get_longitude();
    float lat_diff = end_antenna->get_latitude() -
        start_antenna->get_latitude();
    float lon_diff = end_antenna->get_longitude() -
        start_antenna->get_longitude();

    Path interpolated_path;
    interpolated_path.add_step(start);

    for (unsigned i = 1; i <= num_steps; i++) {
        float interval = (float)i / (float)num_steps;
        float new_lat = start_lat + interval * lat_diff;
        float new_lon = start_lon + interval * lon_diff;

        Antenna* next_antenna = AntennaModel::find_nearest_antenna(new_lat,
            new_lon);
        interpolated_path.add_step(next_antenna->get_id());
    }

    return interpolated_path;
}

Path::Path()
{
    path_iterator = antenna_id_sequence.begin();
}

Path::Path(vector<unsigned> ids)
{
    antenna_id_sequence = ids;
    path_iterator = antenna_id_sequence.begin();
}

void Path::add_step(AntennaId id)
{
    antenna_id_sequence.push_back(id);
}

AntennaId Path::get_next_step(bool from_beginning)
{
    if (from_beginning) {
        path_iterator = antenna_id_sequence.begin();
    }

    if (path_iterator != antenna_id_sequence.end()) {
        return (*path_iterator++);
    } else {
        return -1;
    }

}

AntennaId Path::get_first_step()
{
    if (!antenna_id_sequence.empty()) {
        return antenna_id_sequence.front();
    } else {
        return -1;
    }
}

AntennaId Path::get_last_step()
{
    if (!antenna_id_sequence.empty()) {
        return antenna_id_sequence.back();
    } else {
        return -1;
    }
}
