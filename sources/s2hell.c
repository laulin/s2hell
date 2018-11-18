#include "s2hell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static const s2hell_command * command_list = NULL;
static uint8_t number_of_command = 0;

// set the list of command available in the shell
// it is useful to set a specific set of command regarding the user right
void s2hell_set_command_list(const s2hell_command * commands, uint8_t number)
{
    command_list = commands;
    number_of_command = number;
}

uint8_t s2hell_get_command_list_size()
{
    return number_of_command;
}

const char * s2hell_get_command_name(uint8_t index)
{
    if (index >= number_of_command)
    {
        return NULL;
    }

    return command_list[index].name;
}

uint8_t s2hell_get_command_name_size(uint8_t index)
{
    if (index >= number_of_command)
    {
        return 0;
    }

    return command_list[index].name_size;
}

const char * s2hell_get_command_man(uint8_t index)
{
    if (index >= number_of_command)
    {
        return NULL;
    }

    return command_list[index].man;
}


s2hell_command_function s2hell_get_function_pointer(const char* name, uint8_t size)
{
    for(uint8_t i=0; i < number_of_command; i++)
    {
        if(command_list[i].name_size == size)
        {
            if(memcmp(command_list[i].name, name, size) == 0)
            {
                return command_list[i].function;
            }
        }
    }
    return NULL;
}

s2hell_code s2hell_execute(const char* line, s2hell_token* tokens, uint8_t token_size)
{
    s2hell_argument argv[S2HELL_MAX_ARGUMENT] = {0};
    uint8_t argc = 0;
    uint8_t token_i = 0;
    s2hell_code code = 0;
    s2hell_command_function current_command_function = NULL;

    while(token_i < token_size)
    {
        s2hell_token* token = &tokens[token_i];

        switch(token->type)
        {
            case eol :
                if(current_command_function != NULL)
                {
                    code = current_command_function(argc, argv);
                }
                return code;
            
            // this cas should not happen (filtered by the tokenizer)
            case bad_token :
            case not_defined :
                return S2HELL_EXITCODE_INTERNAL_ERROR;

            case command :
                current_command_function = s2hell_get_function_pointer(&line[token->position], token->length);

                if(current_command_function == NULL)
                {
                    // to be improved
                    return S2HELL_EXITCODE_CANNOT_EXECUTE;
                }
                break;

            case word :
                argv[argc].arg = &line[token->position];
                argv[argc].len = token->length;
                argc++;
            
                break;

            case pipe :
                code = current_command_function(argc, argv);
                if (code != S2HELL_EXITCODE_SUCCESS) // command fail
                {
                    return code;
                }
                argc = 0;
                current_command_function = NULL;
                // the stdout of the previous command begin the stdin of the next command
                s2hell_swap_stds(); 
                s2hell_clear_stdout();
                break;

            default:
                 // should never happen if the dev' rocks
                return S2HELL_EXITCODE_INTERNAL_ERROR;
        }
        token_i++;
    }

    return code;
}