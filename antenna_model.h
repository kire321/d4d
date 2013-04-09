#pragma once

#include "antenna.h"
#include "path.h"

class AntennaModel
{
    multiDimVala<float>* antennas;
    // TODO: want a better way of storing this
    vector<vector<vector< vector<int> > > > transition_frequencies;

    public:
        AntennaModel();
        ~AntennaModel();

        bool add_antenna(AntennaId id, double lat, double lon);
        // TODO: Load/initialize with a file?
        void update(Event& event);
        Antenna* find_antenna_by_id(AntennaId id); // TODO: * or actual objc
        Antenna* find_nearest_antenna(float lat, float lon);

        Path path_prediction(AntennaId start, AntennaId end, unsigned time);

    private:
        unsigned next_step_prediction(AntennaId start, AntennaId end, elapsed_time);
};
