#pragma once

#include <vector>

#include "types.h"

using std::vector;

class Path
{
    vector<unsigned> antenna_id_sequence;
    vector<unsigned>::iterator path_iterator;

    public:
        static Path interpolate_path(AntennaId start, AntennaId end,
            unsigned num_steps);

        Path();
        Path(std::vector<unsigned> ids);

        void add_step(AntennaId id);

        unsigned get_num_steps() { return antenna_id_sequence.size(); };
        // Iterator that returns next step
        AntennaId get_next_step(bool from_beginning = false);
        AntennaId get_first_step();
        AntennaId get_last_step();
};
