#include "s2hell.h"
#include <stdio.h>
#include <string.h>

static s2hell_std std_list[S2HELL_STD_NUMBER] = {0};
static uint8_t current_std = 0;

void s2hell_reset_stds()
{
    current_std = 0;
    for(uint8_t i=0; i < S2HELL_STD_NUMBER ; i++)
    {
        std_list[i].position = 0;
        std_list[i].buffer[0] = '\0';
    }
}

void s2hell_clear_stdout()
{
    std_list[current_std].position = 0;
    std_list[current_std].buffer[0] = '\0';

}


// Return the index to know what is the current stdout 
uint8_t s2hell_get_current_stdout()
{
    return current_std;
}

// Return the index to know what is the current stdin 
uint8_t s2hell_get_current_stdin()
{
    // Warning : it will be not valid if S2HELL_STD_NUMBER > 2 !!!
    return (S2HELL_STD_NUMBER - 1) - current_std; 
}

void s2hell_swap_stds()
{
    // Warning : it will be not valid if S2HELL_STD_NUMBER > 2 !!!
    current_std = (S2HELL_STD_NUMBER - 1) - current_std;
    uint8_t stdout_index = s2hell_get_current_stdout();

    std_list[stdout_index].position = 0;
    std_list[stdout_index].buffer[0] = '\0';
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Three following functions missusing may lead to buffer overflow attack
// Be very careful !
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// it return the current writing for the stdout.
// the size parameter is also an output
// It is recommanded to use s = snprintf(buffer, size, "....");
// /!\ s must be reported in s2hell_stdout_report_write(s)
char * s2hell_get_current_stdout_buffer(uint8_t * size)
{
    uint8_t index = s2hell_get_current_stdout();
    uint8_t position = std_list[index].position;
    *size = S2HELL_STD_SIZE - position -1;
    return &(std_list[index].buffer[position]);
}

// report the number of byte written
void s2hell_stdout_report_write(uint8_t size)
{
    uint8_t index = s2hell_get_current_stdout();
    std_list[index].position += size;
    std_list[index].buffer[std_list[index].position] = 0;
}

// return the complet stdout buffer for [0]
// Do not for write or you will be in trouble (const modifier is NEVER not for ornementation ...)
const char * s2hell_get_complet_stdout_buffer()
{
    uint8_t index = s2hell_get_current_stdout();

    return std_list[index].buffer;
}

void s2hell_stdout_nwrite(const char * source, uint8_t source_size)
{
    if (source == NULL || source_size ==0)
    {
        return ;
    }
    
    uint8_t index = s2hell_get_current_stdout();
    s2hell_std * current_stdout = &std_list[index];
    uint8_t position = current_stdout->position;
    uint8_t stdout_size =  S2HELL_STD_SIZE - position -1;
    uint8_t bytes_to_copy = 0;

    // prevent buffer overflow
    if(source_size > stdout_size)
    {
        bytes_to_copy = stdout_size;
    }
    else
    {
        bytes_to_copy = source_size;
    }

    memcpy(&(current_stdout->buffer[position]), source, bytes_to_copy);

    position += bytes_to_copy;
    current_stdout->position = position;
    current_stdout->buffer[position] = 0;
}

void s2hell_stdout_write(const char * source)
{
    uint8_t source_size = strlen(source);
    s2hell_stdout_nwrite(source, source_size);
}

// ------

// return the complet stdin buffer for [0]
// Do not for write or you will be in trouble (const modifier is NEVER not for ornementation, again)
const char * s2hell_get_complet_stdin_buffer()
{
    uint8_t index = s2hell_get_current_stdin();

    return std_list[index].buffer;
}

uint8_t s2hell_get_stdin_size()
{
    uint8_t index = s2hell_get_current_stdin();

    return std_list[index].position; 
}