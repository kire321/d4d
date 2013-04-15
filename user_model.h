class UserModel
{
    map<UserId, User*> users;

    public:
        UserModel();
        UserModel(FILE* event_data);

        bool add_user(UserId id);
        void update(Event& event);
        User* find_by_id(UserId id);
}
