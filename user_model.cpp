#include "user_model.h"

map<UserId, User*> UserModel::users = map<UserId, User*>();

void UserModel::init()
{
}

void UserModel::clear()
{
    users.clear();
}

bool UserModel::add_user(UserId id)
{
    if (!find_user_by_id(id)) {
        users[id] = new User(id);
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
    if (users.find(id) != users.end()) {
        return users[id];
    } else {
        return NULL;
    }
}
