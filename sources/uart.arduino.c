#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include "s2hell.h"

#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

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

void on_new_line(const unsigned char* buffer, unsigned char size)
{
    print_buffer("->", 3);
    print_buffer(buffer, size);
    print_buffer("\n\r", 2);
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
    
    while(1) {

        if(m_UART_received())
        {
            // minicom -c on -b 9600 -D /dev/ttyACM0
            received_value = m_UART_read();
            s2hell_process_terminal(received_value);

            
        }
    }
}
