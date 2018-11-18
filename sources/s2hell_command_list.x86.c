#include "s2hell.h"

#define S2HELL_DEFAULT_NUMBER_OF_COMMAND (4)

static const s2hell_command S2HELL_DEFAULT_COMMANDS[S2HELL_DEFAULT_NUMBER_OF_COMMAND] = {
    {
        .name = "lscmd",
        .name_size = 5,
        .function = default_lscmd,
        .man = man_lscmd
    },
    {
        .name = "man",
        .name_size = 3,
        .function = default_man,
        .man = man_man
    },
    {
        .name = "search",
        .name_size = 6,
        .function = default_search,
        .man = man_search
    },
    {
        .name = "echo",
        .name_size = 4,
        .function = default_echo,
        .man = man_echo
    }
};

void s2hell_set_default_x86_commands()
{
    s2hell_set_command_list(S2HELL_DEFAULT_COMMANDS, S2HELL_DEFAULT_NUMBER_OF_COMMAND);
}