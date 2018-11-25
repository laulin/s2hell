#include "s2hell.h"
#include <stdio.h>
#include <string.h>

s2hell_code default_lscmd(uint8_t argc, s2hell_argument* argv)
{
    uint8_t number_of_available_commands = s2hell_get_command_list_size();
    for(uint8_t i=0; i < number_of_available_commands; i++)
    {
        s2hell_stdout_write(s2hell_get_command_name(i));
        s2hell_stdout_write("\n\r");
    }
    return S2HELL_EXITCODE_SUCCESS;
}

s2hell_code default_man(uint8_t argc, s2hell_argument* argv)
{
    if (argc != 1)
    {
        return S2HELL_EXITCODE_BAD_NUMBER_OF_ARGS;
    }

    uint8_t number_of_available_commands = s2hell_get_command_list_size();
    for(uint8_t i=0; i < number_of_available_commands; i++)
    {
        if(s2hell_get_command_name_size(i) == argv->len)
        {
            if(memcmp(argv->arg, s2hell_get_command_name(i), argv->len) == 0)
            {
                s2hell_stdout_write(s2hell_get_command_man(i));
                return S2HELL_EXITCODE_SUCCESS;
            }
        }
    }

    s2hell_stdout_write("Not man found for this command\n\r");
    return S2HELL_EXITCODE_SUCCESS;
}

s2hell_code default_search(uint8_t argc, s2hell_argument* argv)
{
    if (argc != 1)
    {
        s2hell_stdout_write("only one arg is needed\n\r");
        return S2HELL_EXITCODE_BAD_NUMBER_OF_ARGS;
    }

    const char * stdin_buffer = s2hell_get_complet_stdin_buffer();
    uint8_t stdin_size = s2hell_get_stdin_size();
    uint8_t line_begining_position = 0;
    uint8_t found = 0;
    uint8_t compare_index = 0;

    for(int8_t i=0; i < stdin_size+1; i++)
    {
        if(stdin_buffer[i] == '\n' || i == stdin_size)
        {
            if(found)
            {
                s2hell_stdout_nwrite(&stdin_buffer[line_begining_position], i - line_begining_position);
                s2hell_stdout_write("\n\r");
                found = 0;
            }
            line_begining_position = i+1;
            compare_index = 0;
            continue;
        }

        if (stdin_buffer[i] == argv->arg[compare_index])
        {
            if(compare_index == argv->len-1)
            {
                found = 1;
                compare_index = 0;
                continue;
            }
            compare_index++;
        }
        else
        {
            compare_index = 0;
        }
    }
    return S2HELL_EXITCODE_SUCCESS;
}

s2hell_code default_echo(uint8_t argc, s2hell_argument* argv)
{
    if (argc != 1)
    {
        s2hell_stdout_write("only one arg is needed\n\r");
        return S2HELL_EXITCODE_BAD_NUMBER_OF_ARGS;
    }

    s2hell_stdout_nwrite(argv->arg, argv->len);
    s2hell_stdout_write("\n\r");

    return S2HELL_EXITCODE_SUCCESS;
}