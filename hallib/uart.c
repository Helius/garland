#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"

#ifndef UART_BAUDRATE
#pragma error "uart UART_BAUDRATE was not set!!"
#else
#define XSTR(x) STR(x)
#define STR(x) #x
#pragma message("Use uart baudrate "XSTR(UART_BAUDRATE))
#endif


#if defined UART_DOUBLE_SPEED
#define UART_DIVIDER 8UL
#pragma message("Use uart double speed mode ")
#else
#define UART_DIVIDER 16UL
#endif

#define BAUD_PRESCALLER (((F_CPU / (UART_BAUDRATE * UART_DIVIDER))) - 1)

int uart_putchar(char c, FILE *stream) {
#if defined __AVR_ATmega328P__ || defined __AVR_ATmega168__
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
#endif
#ifdef __AVR_ATmega8__
	while(!(UCSRA & (1<<UDRE)));
	UDR = c;
#endif 
	return 0;
}

int uart_getchar(FILE *sttream) {
#if defined __AVR_ATmega328P__ || defined __AVR_ATmega168__
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
#endif
#ifdef __AVR_ATmega8__
	while(!(UCSRA & (1<<RXC)));
	return UDR;
#endif
	return 0;
}

static void (*receiver_handler)(unsigned char) = 0;

void set_receive_interrupt_handler(void (*handler)(unsigned char))
{
	receiver_handler = handler;
	UCSR0B |= 1 << RXCIE0;
}

ISR(USART_RX_vect)
{
	if (receiver_handler != 0) {
		(*receiver_handler)(UDR0);
	}
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void uart_init(void) {
#if defined __AVR_ATmega328P__ || defined __AVR_ATmega168__
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));
#if defined UART_DOUBLE_SPEED
	UCSR0A = (1<<U2X0);
#endif
#elif defined __AVR_ATmega8__
	UBRRH = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRRL = (uint8_t)(BAUD_PRESCALLER);
	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = ((1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1));
#if defined UART_DOUBLE_SPEED
	UCSRA = (1<<U2X);
#endif
#else
#pragma error "can't find configuration for this MC"
#endif

	stdout = &uart_output;
	stdin  = &uart_input;
}

