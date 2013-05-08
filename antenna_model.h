#pragma once

#include "antenna.h"
#include "path.h"
#include "types.h"

#include <vector>
#include <map>
#include <valarray>
#include <fstream>

using std::vector;
using std::map;
using std::valarray;
using std::ifstream;

class AntennaModel
{
    public:
        // Members
        static map<AntennaId, Antenna*> antennas;
        static map<AntennaId, map<AntennaId, map<unsigned, vector<AntennaId> > > > transitions;
        static int timestep;

        // Initialize
        static void init(ifstream& file, int step);

        // Modify/access
        static bool add_antenna(valarray<float> antenna_data);
        static void update(Event* event);
        static Antenna* find_antenna_by_id(AntennaId id);
        static Antenna* find_nearest_antenna(float lat, float lon);

        static AntennaId predict_location(AntennaId start, AntennaId end,
            unsigned num_steps, unsigned path_len);

    private:

        static AntennaId next_step_prediction(AntennaId start, AntennaId end, unsigned num_steps);
        // static AntennaId next_step_prediction(AntennaId current, unsigned elapsed_time);
};
