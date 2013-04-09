#pragma once

#include "antenna.h"
#include "path.h"

class AntennaModel
{
    multiDimVala<float>* antennas;
    vector<vector<vector< vector<int> > > > transition_frequencies;

    public:
        AntennaModel();
        ~AntennaModel();

        bool add_antenna(unsigned id, double lat, double lon);
        // TODO: Load/initialize with a file?
        void update(Event& event);
        Antenna* find_antenna_by_id(unsigned id); // TODO: * or actual objc

        Path path_prediction(unsigned start, unsigned end, unsigned time);

    private:
        unsigned next_step_prediction(unsigned start, unsigned end, elapsed_time);
};
