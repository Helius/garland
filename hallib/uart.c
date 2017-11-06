#include <stdio.h>
#include <avr/io.h>

#include "uart.h"

#ifndef UART_BAUDRATE
#pragma error "uart UART_BAUDRATE was not set!!"
#else
#define XSTR(x) STR(x)
#define STR(x) #x
#pragma message( "Use uart baudrate "XSTR(UART_BAUDRATE))
#endif

#define BAUD_PRESCALLER (((F_CPU / (UART_BAUDRATE * 16UL))) - 1)

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

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void uart_init(void) {
#if defined __AVR_ATmega328P__ || defined __AVR_ATmega168__
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));
#elif defined __AVR_ATmega8__
	UBRRH = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRRL = (uint8_t)(BAUD_PRESCALLER);
	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = ((1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1));
#else
#pragma error "can't find configuration for this MC"
#endif

	stdout = &uart_output;
	stdin  = &uart_input;
}

