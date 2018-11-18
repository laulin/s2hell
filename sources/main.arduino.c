#ifndef F_CPU
#define F_CPU 16000000UL // or whatever may be your frequency
#endif
 
#include <avr/io.h>
#include <util/delay.h>                // for _delay_ms()
#include "s2hell.h"

#define MAX_TOKEN 10
const char * COMMAND = "help";
 
int main(void)
{
    // NOT TESTED YET !
    s2hell_token tokens[MAX_TOKEN] = {0};
    s2hell_set_default_arduino_commands();
    DDRB = 0x20;                       // initialize port D
    while(1)
    {
        int token_number = s2hell_tokenize(COMMAND, tokens, MAX_TOKEN);
        if (token_number > 0)
        {
            s2hell_code exitstatus = s2hell_execute(COMMAND, tokens, MAX_TOKEN);
            //printf("%s", s2hell_get_complet_stdout_buffer());
            //printf("\nexitstatus : %d\n", exitstatus);
        }
        // LED on
        PORTB = 0x20 ^ PORTB;            // PD0 = High = Vcc

    }
}