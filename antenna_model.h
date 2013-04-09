class AntennaModel
{
    vector<vector<vector<int> > > transition_frequencies;

    public:
        AntennaModel();
        AntennaModel(FILE* antenna_data);

        bool add_antenna(unsigned id, double lat, double long);
        bool load(FILE* event_data);

        void update(Event& event);
        unsigned interpolate_path(unsigned start, unsigned end, unsigned time); // How do we decide partial time? do we output barycentric weighting?

        unsigned next_step_prediction(unsigned start, unsigned end, unsigned time);
};
