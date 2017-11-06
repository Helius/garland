#ifndef _uart_h
#define _uart_h 1

#include <stdio.h>

void uart_init(void);
void set_receive_interrupt_handler(void (*handler)(unsigned char));

#endif /* defined _uart_h */
