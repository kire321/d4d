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

class AntennaModel
{
    // multiDimVala<float>* antennas; // TABLE

    public:
        // Members
        static map<AntennaId, Antenna*> antennas;
        static map<AntennaId, map<AntennaId, map<unsigned, map<AntennaId,
            unsigned> > > > transition_frequencies;

        // Initialize
        static void init(ifstream& file);

        // Modify/access
        static bool add_antenna(valarray<float> antenna_data);
        static void update(Event* event);
        static Antenna* find_antenna_by_id(AntennaId id); // TODO: * or actual objc
        static Antenna* find_nearest_antenna(float lat, float lon);

        static Path path_prediction(AntennaId start, AntennaId end, unsigned time);
        static Path path_prediction(AntennaId current, unsigned time);

        // void print_statistics(ofstream& file);

    private:

        static AntennaId next_step_prediction(AntennaId start, AntennaId end, unsigned elapsed_time);
        static AntennaId next_step_prediction(AntennaId current, unsigned elapsed_time);
};
