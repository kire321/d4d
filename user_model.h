class UserModel
{
    map<unsigned, User> users;

    public:
        UserModel();
        UserModel(FILE* event_data);

        void update(Event& event);
}
