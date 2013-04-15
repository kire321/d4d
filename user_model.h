#include "user.h"
#include "types.h"
#include <map>

using std::map;

class UserModel
{
    map<UserId, User*>* users;

    public:
        UserModel();
        ~UserModel();

        bool add_user(UserId id);
        void update(Event* event);
        User* find_user_by_id(UserId id);
};
