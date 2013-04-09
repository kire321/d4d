#pragma once

#include <vector>

#include "types.h"

class Path
{
    std::vector<unsigned> antenna_id_sequence;
    std::vector<unsigned>::iterator path_iterator;

    public:
        static Path interpolate_path(AntennaId start, AntennaId end,
            unsigned time); // How do we decide partial time? do we output barycentric weighting?

        Path();
        Path(std::vector<unsigned> ids, unsigned time);
        Path operator=(const Path& other);

        void add_step(AntennaId id);

        unsigned get_elapsed_time() { return antenna_id_sequence.size(); };
        // Iterator that returns next step
        unsigned get_next_step(bool from_beginning = false);
        unsigned get_last_step()
        {
            return antenna_id_sequence.size() && antenna_id_sequence.last();
        }
};
