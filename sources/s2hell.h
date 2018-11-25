#ifndef __S2HELL_HEADER__
#define __S2HELL_HEADER__

#include <stdint.h>

#define NO_ERROR 0
#define ERROR -1

// interesting point : https://www.tldp.org/LDP/abs/html/exitcodes.html#EXITCODESREF

#define S2HELL_EXITCODE_SUCCESS             0
#define S2HELL_EXITCODE_BAD_NUMBER_OF_ARGS  1
#define S2HELL_EXITCODE_BAD_ARG             2
#define S2HELL_EXITCODE_CANNOT_EXECUTE      126
#define S2HELL_EXITCODE_INTERNAL_ERROR      255


// === Configuration ===
// Please take care of that or you may be out-of-memory !
#define S2HELL_MAX_ARGUMENT (16) // define the maximum number of argument per command. default is 16
#define S2HELL_STD_SIZE (128) // define the maximum size of stdout buffer. Default is 128, can be greater than *254*
#define S2HELL_STD_NUMBER (2) // define the number of STD buffer; Currently it *must* not be different of 2 ! (you have be warned)
// =====================

typedef struct _argument
{
    const char * arg;
    uint8_t len;
}
s2hell_argument;

typedef struct _std  s2hell_std; // prevent missing definition, done in s2hell_std.h

typedef uint8_t s2hell_code;
// stdin, stdout, argc, argv -> int
typedef s2hell_code (*s2hell_command_function)(uint8_t, s2hell_argument*);


typedef struct _command
{
    const char * name;
    uint8_t name_size;
    s2hell_command_function function;
    const char * man;
}
s2hell_command;


#include "s2hell_std.h"
#include "s2hell_tokenizer.h"
#include "s2hell_commands.h"
#include "s2hell_terminal.h"

void s2hell_set_command_list(const s2hell_command * commands, uint8_t number);
uint8_t s2hell_get_command_list_size();
const char * s2hell_get_command_name(uint8_t);
uint8_t s2hell_get_command_name_size(uint8_t index);
const char * s2hell_get_command_man(uint8_t);
s2hell_command_function s2hell_get_function_pointer(const char* name, uint8_t size);
s2hell_code s2hell_execute(const char* line, s2hell_token* tokens, uint8_t token_size);

#ifndef __AVR_ATmega328P__
#include "s2hell_command_list.x86.h"
#else
#include "s2hell_command_list.arduino.h"
#endif


#endif