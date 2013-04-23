#include "user_model.h"

UserModel::UserModel()
{
    users = new map<UserId, User*>();
}

UserModel::~UserModel()
{
    delete users;
}

bool UserModel::add_user(UserId id)
{
    if (!find_user_by_id(id)) {
        users->at(id) = new User(id);
        return true;
    }
    return false;
}

void UserModel::update(Event* event)
{
    add_user(event->user_id);
    User* user = find_user_by_id(event->user_id);

    user->add_event(event);
}

User* UserModel::find_user_by_id(UserId id)
{
    return users->at(id);
}

void UserModel::print_statistics(ofstream& file) const
{
    const
    // Basically iterate per user
    // for each user iterate over real events
    // try to find a matching prediction
    // keep a count of covered vs total # events
    // keep a count of correct incorrect
  //TODO
}
