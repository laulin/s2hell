#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

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

#define CHARACTER 0
#define ESCAPE 1
#define NOT_ASCII 2
#define CSI_PARAMETERS 3
#define CSI_INTERMEDIATES 4
#define CSI_FINAL 5

#define BUFFER_SIZE 80
unsigned char g_buffer_size = 0;
char g_buffer[BUFFER_SIZE] = {0};
unsigned char g_state = 0;
unsigned char g_cursor = 0;


void print_buffer(const unsigned char* buffer, unsigned char size)
{
    unsigned char i=0;

    for(i=0; i<size; i++)
    {
        m_UART_wait_transmitted();
        m_UART_write(buffer[i]);
    }
}

void print_char(unsigned char c)
{
    m_UART_wait_transmitted();
    m_UART_write(c);
}

void process(unsigned char x)
{
    switch(g_state)
    {
        case CHARACTER:
            if(x >= 127) // utf8
            {
                g_state = NOT_ASCII;
                break;
            }
            if(x == 27) // escape
            {
                g_state = ESCAPE;
                break;
            }
            if(x == 8) // <-
            {
                if(g_buffer_size > 0) g_buffer_size--;
                print_buffer("\x1b[D", 3);
                print_buffer("\x1b[K", 3);

                break;
            }
            if(x == 13) // return
            {
                // print_buffer("\x1b[2K", 4);
                // print_buffer("\r", 1);
                // print_buffer(g_buffer, g_buffer_size);
                print_buffer("\n\r", 2);
                print_buffer("->", 2);
                print_buffer("\x1b[31m", 5);
                print_buffer(g_buffer, g_buffer_size);
                print_buffer("\x1b[m", 3);
                print_buffer("\n\r", 2);
                g_buffer_size = 0;
                break;
            }
            if(g_buffer_size >= BUFFER_SIZE) 
            {
                break;
            }

            if(x >= 32 && x < 127) 
            {
                g_buffer[g_buffer_size] = x;
                print_char(x);
                g_buffer_size++;
            }
            else
            {
                unsigned char s = 0;
                
                s = snprintf(&g_buffer[g_buffer_size], BUFFER_SIZE-g_buffer_size, "0x%02X", x);
                print_buffer(&g_buffer[g_buffer_size], s);
                g_buffer_size +=s;
            }
            break;

        case ESCAPE:
            if(x == '[')
            {
                g_state = CSI_PARAMETERS;
            }
            else
            {
                g_state = CHARACTER;
            }
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
            if(x >= 0x40 && x <= 0x7E)
            {
                //finall byte
            }
            g_state = CHARACTER;
            break;

        case NOT_ASCII:
            // nothing to do
            g_state = CHARACTER;
            break;

        default:
            break;
    }
    // print_buffer("\x1b[2K", 4);
    // print_buffer("\r", 1);
    // print_buffer(g_buffer, g_buffer_size);

}

int main (void)
{
    USART_Init(UBRR_VALUE);

    uint8_t received_value = 0xAA;
    DDRB = 0x20;                       // initialize port D
    while(1) {

        if(m_UART_received())
        {
            received_value = m_UART_read();
            process(received_value);

            
        }
    }
}
