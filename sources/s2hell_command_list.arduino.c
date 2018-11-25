#include "s2hell.h"
#include <stdio.h>
#include <avr/io.h>

#define S2HELL_DEFAULT_NUMBER_OF_COMMAND (5)

#define Not_man "not availble\n"

/* 

display state of direction

example : 

D0:0
D1:0
D2:0
D3:0
D4:0
D5:0
D6:0
D7:0
D8:0
D9:0
D10:0
D11:0
D12:0
D13:0
*/
s2hell_code default_getdp(uint8_t argc, s2hell_argument* argv)
{
    char tmp[15] = {0};

    for(uint8_t i=0; i<8; i++)
    {
        uint8_t masked = DDRD & (1<<i) != 0;
        snprintf(tmp, 15, "D%d:%d\0", i, masked);
        s2hell_stdout_write(tmp);
        s2hell_stdout_write("\n\r");
    }

    for(uint8_t i=0; (i+8) < 14; i++)
    {
        uint8_t masked = DDRB & (1<<i) != 0;
        snprintf(tmp, 15, "D%d:%d\0", i+8, masked);
        s2hell_stdout_write(tmp);
        s2hell_stdout_write("\n\r");
    }

    return S2HELL_EXITCODE_SUCCESS;
}

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
    },
    {
        .name = "getdp",
        .name_size = 5,
        .function = default_getdp,
        .man = Not_man
    }
};

void s2hell_set_default_arduino_commands()
{
    s2hell_set_command_list(S2HELL_DEFAULT_COMMANDS, S2HELL_DEFAULT_NUMBER_OF_COMMAND);
}