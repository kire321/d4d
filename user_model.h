#pragma once

#include "user.h"
#include "types.h"
#include <map>
#include <fstream>

using std::map;

class UserModel
{
    public:
        static map<UserId, User*> users;

        static void init();
        static void clear(); // Use to destroy records of users

        static bool add_user(UserId id);
        static void update(Event* event);
        static User* find_user_by_id(UserId id);

        // void print_statistics(ofstream& file) const;
};
