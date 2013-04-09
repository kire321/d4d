#pragma once

class Path
{
    std::vector<unsigned> antenna_id_sequence;
    std::vector<unsigned>::iterator path_iterator;

    public:
        static unsigned interpolate_path(unsigned start, unsigned end, unsigned time); // How do we decide partial time? do we output barycentric weighting?

        Path();
        Path(std::vector<unsigned> ids, unsigned time);
        Path operator=(const Path& other);

        // Iterator that returns next step
        unsigned get_elapsed_time() { return sequence.size(); };
        unsigned get_next_step(bool from_beginning = false);
        unsigned get_last_step() { return sequence.size() && sequence.last(); };
};
