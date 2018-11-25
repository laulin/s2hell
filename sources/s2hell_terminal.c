#include "s2hell.h"
#include <stdio.h>
#include <string.h>

#define CHARACTER 0
#define ESCAPE 1
#define NOT_ASCII 2
#define CSI_PARAMETERS 3
#define CSI_INTERMEDIATES 4
#define CSI_FINAL 5

static uint8_t g_buffer_size = 0;
static uint8_t g_buffer[S2HELL_TERMINAL_BUFFER_SIZE+1] = {0};
static uint8_t g_cursor = 0;
static uint8_t g_state = CHARACTER;
// callbacks definition
void (*g_print_buffer)(const uint8_t* buffer, uint8_t size) = NULL;
void (*g_print_char)(uint8_t c) = NULL;
void (*g_on_new_line)(const uint8_t * buffer, uint8_t size) = NULL;


void s2hell_clear_line()
{
    g_buffer_size = 0;
    g_cursor = 0;
    memset(g_buffer, 0, S2HELL_TERMINAL_BUFFER_SIZE);
}

uint8_t* s2hell_get_line_buffer()
{
    return g_buffer;
}

uint8_t s2hell_get_line_size()
{
    return g_buffer_size;
}

uint8_t s2hell_add_character(uint8_t x)
{
    if(g_buffer_size >= S2HELL_TERMINAL_BUFFER_SIZE) // prevent buffer overflow
    {
        return ERROR;
    }

    if(x < 32 || x >= 127) // only add printable ascii
    {
        return ERROR;
    }
    
    if(g_cursor == g_buffer_size)
    {
        g_buffer[g_buffer_size] = x;
        g_buffer_size++;
        g_cursor++;

        return NO_ERROR;
    }

    if(g_cursor < g_buffer_size) // wrap beahaviour
    {
        g_buffer[g_cursor] = x;
        g_cursor++;

        return NO_ERROR;
    }
    
    return ERROR;
}

void s2hell_move_cursor_backward(uint8_t n)
{
    if(n >= g_cursor)
    {
        g_cursor = 0;
    }
    else
    {
        g_cursor -= n;
    }
}

void s2hell_move_cursor_forward(uint8_t n)
{
    if(g_cursor + n >= S2HELL_TERMINAL_BUFFER_SIZE)
    {
        g_cursor = S2HELL_TERMINAL_BUFFER_SIZE;
    }
    else
    {
        g_cursor += n;
    }
}

void s2hell_backspace()
{
    if(g_cursor == 0 || g_buffer_size == 0)
    {
        return;
    }

    if(g_cursor == g_buffer_size)
    {
        g_cursor--;
        g_buffer_size--;
        return ;
    }

    memcpy(&g_buffer[g_cursor-1], &g_buffer[g_cursor], g_buffer_size - g_cursor);
    g_buffer[g_cursor] = 0;
    g_cursor--;
    g_buffer_size--;
}

void s2hell_delete()
{
    if(g_cursor == g_buffer_size || g_buffer_size == 0)
    {
        return;
    }

    memcpy(&g_buffer[g_cursor], &g_buffer[g_cursor+1], g_buffer_size - g_cursor);
    g_buffer[g_cursor] = 0;
    g_cursor--;
    g_buffer_size--;
}

void call_print_buffer(const char * buffer, uint8_t size)
{
    if(g_print_buffer != NULL)
    {
        g_print_buffer((const uint8_t*)buffer, size);
    }
}

void call_print_char(char c)
{
    if(g_print_char != NULL)
    {
        g_print_char(c);
    }
}

void call_on_new_line(const char * buffer, uint8_t size)
{
    if(g_on_new_line != NULL)
    {
        g_on_new_line((const uint8_t*)buffer, size);
    }
}

void on_character(uint8_t x)
{
    if(x < 32)
    {
        //control
        switch(x)
        {
            case 8: // back space
                {
                    s2hell_backspace();
                    call_print_buffer("\x1b[D", 3);
                    call_print_buffer("\x1b[K", 3);
                    break;
                }
            /*case 9: // tab
                {
                    break;
                }*/
            case 13: // return
                {
                    // rewrite the complet line
                    call_print_char('\r');
                    call_print_buffer((const char *)g_buffer, g_buffer_size);
                    
                    // go to the next line
                    call_print_buffer("\r\n", 2);

                    // run
                    call_on_new_line((const char*)g_buffer, g_buffer_size);
                    s2hell_clear_line();
                    break;
                }
            case 27: // escape
                {
                    g_state = ESCAPE;
                    break;
                }
            default:
                // nothing to do
                break;
        }
    }
    else if (x>= 32 && x<127)
    {
        // ascii
        s2hell_add_character(x);
        call_print_char(x);
    }
    else
    {
        // not ascii
        g_state = NOT_ASCII;
    }
}

void on_escape(uint8_t x)
{
    if(x == '[')
    {
        g_state = CSI_PARAMETERS;
    }
    else
    {
        g_state = CHARACTER;
    }
}

void on_csi_final(uint8_t x)
{
    if(x >= 0x40 && x <= 0x7E)
    {
        //finall byte
        if(x == 'D') // cursor backward
        {
            call_print_buffer("\x1b[D", 3);
            if(g_cursor > 0) g_cursor--;
        }
        if(x == 'C') // cursor forward
        {
            call_print_buffer("\x1b[C", 3);
            if(g_cursor < g_buffer_size) g_cursor++;
        }
        if(x == '~') // minicom provide "del" like that
        {
            call_print_buffer("del", 3);
            s2hell_delete();
        }
    }
    g_state = CHARACTER;
}

void s2hell_process_terminal(uint8_t x)
{
    switch(g_state)
    {
        case CHARACTER:
            on_character(x);
            break;

        case ESCAPE:
            on_escape(x);
            break;

        case CSI_PARAMETERS:
            // https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_sequences
            if(x >= 0x30 && x <= 0x3F)
            {
                //parameters bytes
                break;
            }
            g_state = CSI_INTERMEDIATES;
        
        case CSI_INTERMEDIATES:
            if(x >= 0x20 && x <= 0x2F)
            {
                //intermediate bytes
                break;
            }
            g_state = CSI_FINAL;

        case CSI_FINAL:
            on_csi_final(x);
            break;

        case NOT_ASCII:
        default:
            g_state = CHARACTER;
    }
}

void set_print_buffer(void (*print_buffer)(const uint8_t* buffer, uint8_t size))
{
    g_print_buffer = print_buffer;
}

void set_print_char(void (*print_char)(uint8_t c))
{
    g_print_char = print_char;
}

void set_on_new_line(void (*on_new_line)(const uint8_t * buffer, uint8_t size))
{
    g_on_new_line = on_new_line;
}
