#include "user_model.h"

#include "user.h"

UserModel::UserModel()
{
    users = new map<UserId, User*>();
}

UserModel::~UserModel()
{
    delete users;
}

bool add_user(UserId id)
{
    if (!find_user_by_id(id)) {
        users[id] = new User();
        return true;
    }
    return false;
}

void update(Event* event)
{
    add_user(event->user_id);
    User* user = find_user_by_id(event->user_id);

    user->add_event(event);
}
