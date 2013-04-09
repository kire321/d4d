#include <cstdlib>
#include <cstdio>

int main(int argc, char** argv)
{
    // FILE* data_file;
    // if (argc == 1) {
    //     char* data_filename = argv[1];
    //     if (data_file = fopen(data_filename, "r+")) {
    //     } else {
    //        perror("Could not open file");
    //     }
    // } else if (argc > 1) {
    //     fprintf(stderr, "Usage: predictor [data_file]");
    // }

    Table* event_table = new Table("Events"); // Table that stores all events
    event_table->initialize(event_data_file);
    Table* antenna_table = new Table("Antenna"); // Stores antenna, lat/lon
    antenna_table->initialize(antenna_data_file);

    valarray<user*> users = event_table->get_all_users();

    // Fork off/initialize other processes
    enter_event_loop();

    return EXIT_SUCCESS;
}

void enter_event_loop()
{
    // Will fork a child
    // Eventually, rewire stdin from parent process to use some input stream
    // Rewire stdout as well to file - print uid, time, actual, predicted, delta
    // we define
}

void handle_new_input(char* event_string)
{
    // Parse it
    Event new_event = parse(event_string);
    event_table->add_event(new_event); // Add event should mark a user as dirty
    User* user = users->get_by_uid(new_event.get_user());
    user->make_prediction(new_event);
}

void process_new_event()
{
}
