#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "s2hell.h"

#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define MAX_TOKEN 32

void USART_Init( unsigned int ubrr)
{
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

#define m_UART_transmitted() (UCSR0A & (1<<UDRE0))
#define m_UART_received() (UCSR0A & (1<<RXC0))
#define m_UART_read() (UDR0)
#define m_UART_write(x) (UDR0 = x)
#define m_UART_wait_transmitted() {while(!m_UART_transmitted()) {}}


void print_buffer(const unsigned char* buffer, unsigned char size)
{
    unsigned char i=0;

    for(i=0; i<size; i++)
    {
        m_UART_wait_transmitted();
        m_UART_write(buffer[i]);
    }
}

void print_error(const unsigned char* buffer)
{
    print_buffer("\x1B[97;41m", 8);
    print_buffer(buffer, strlen(buffer));
    print_buffer("\x1B[m\r\n", 5);
}

void on_new_line(const unsigned char* buffer, unsigned char size)
{
    s2hell_token tokens[MAX_TOKEN] = {0};
    uint8_t token_number = s2hell_tokenize(buffer, tokens, MAX_TOKEN);
    if (token_number > 0)
    {
        s2hell_code exitstatus = s2hell_execute(buffer, tokens, MAX_TOKEN);

        const uint8_t* stdout_buffer = s2hell_get_complet_stdout_buffer();
        uint8_t stdout_size = strlen(stdout_buffer);
        if(exitstatus == S2HELL_EXITCODE_SUCCESS)
        {
            print_buffer(stdout_buffer, stdout_size);
        }
        else
        {

            switch(exitstatus)
            {
                case S2HELL_EXITCODE_BAD_NUMBER_OF_ARGS :
                    print_error("bad number of arguments");
                    break;
                case S2HELL_EXITCODE_BAD_ARG :
                    print_error("bad argument(s)");
                    break;
                case S2HELL_EXITCODE_CANNOT_EXECUTE :
                    print_error("cannot execute");
                    break;
                case S2HELL_EXITCODE_INTERNAL_ERROR :
                    print_error("internal error");
                    break;            
            }
            print_buffer("\x1B[31m", 5);
            print_buffer(stdout_buffer, stdout_size);
            print_buffer("\x1B[m", 3);
        }
    }
}

void print_char(unsigned char c)
{
    m_UART_wait_transmitted();
    m_UART_write(c);
}


int main (void)
{
    USART_Init(UBRR_VALUE);

    uint8_t received_value = 0xAA;

    set_print_buffer(print_buffer);
    set_on_new_line(on_new_line);
    set_print_char(print_char);

    s2hell_set_default_arduino_commands();
    
    while(1) {

        if(m_UART_received())
        {
            // minicom -c on -b 9600 -D /dev/ttyACM0
            received_value = m_UART_read();
            s2hell_process_terminal(received_value);

            
        }
    }
}
