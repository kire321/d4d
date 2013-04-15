#pragma once

#include "antenna.h"
#include "path.h"
#include "types.h"

#include <vector>
#include <map>
#include <valarray>

using std::vector;
using std::map;
using std::valarray;

class AntennaModel
{
    // multiDimVala<float>* antennas; // TABLE
    vector<Antenna*>* antennas;
    map<AntennaId, unsigned>* antenna_id_map; // map of external id to table id
    valarray<valarray<valarray<valarry<unsigned> > > >* transition_frequencies;

    public:
        AntennaModel();
        AntennaModel(ifstream& file);
        ~AntennaModel();

        bool add_antenna(valarray<float> antenna_data);
        void update(Event& event);
        Antenna* find_antenna_by_id(AntennaId id); // TODO: * or actual objc
        Antenna* find_nearest_antenna(float lat, float lon);

        Path path_prediction(AntennaId start, AntennaId end, unsigned time);
        Path path_prediction(AntennaId current, unsigned time);

    private:
        init();

        unsigned next_step_prediction(AntennaId start, AntennaId end, unsigned elapsed_time);
        unsigned next_step_prediction(AntennaId current, unsigned elapsed_time);
};
