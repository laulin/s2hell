#include "s2hell.h"

#define S2HELL_DEFAULT_NUMBER_OF_COMMAND (4)

#define Not_man "not availble\n"


static const s2hell_command S2HELL_DEFAULT_COMMANDS[S2HELL_DEFAULT_NUMBER_OF_COMMAND] = {
    {
        .name = "lscmd",
        .name_size = 5,
        .function = default_lscmd,
        .man = Not_man
    },
    {
        .name = "man",
        .name_size = 3,
        .function = default_man,
        .man = Not_man
    },
    {
        .name = "search",
        .name_size = 6,
        .function = default_search,
        .man = Not_man
    },
    {
        .name = "echo",
        .name_size = 4,
        .function = default_echo,
        .man = Not_man
    }
};

void s2hell_set_default_arduino_commands()
{
    s2hell_set_command_list(S2HELL_DEFAULT_COMMANDS, S2HELL_DEFAULT_NUMBER_OF_COMMAND);
}