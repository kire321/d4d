#include "path.h"
#include "globals.h"
#include "antenna_model.h"
#include "antenna.h"

Path Path::interpolate_path(AntennaId start, AntennaId end,
    unsigned elapsed_time)
{
    Antenna* start_antenna = g_antenna_model->get_antenna_by_id(start);
    Antenna* end_antenna = g_antenna_model->get_antenna_by_id(end);

    float start_lat = start_antenna->lat();
    float start_lon = start_antenna->lon();
    float lat_diff = end_antenna->lat() - start_antenna->lat();
    float lon_diff = end_antenna->lon() - start_antenna->lon();

    Path interpolated_path;
    interpolated_path.add_step(start);

    for (unsigned i = 1; i <= elapsed_time; i++) {
        float interval = (float)i / (float)elapsed_time;
        float new_lat = start_lat + interval * lat_diff;
        float new_lon = start_lon + interval * lon_diff;

        Antenna* next_antenna = AntennaModel->find_nearest_antenna(new_lat,
            new_lon);
        interpolated_path.add_step(next_antenna->get_id());
    }

    return interpolated_path;
}
