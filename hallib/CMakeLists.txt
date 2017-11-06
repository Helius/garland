add_definitions("-DUART_BAUDRATE=19200")

add_avr_library(
   hallib
   uart.c 
   uart.h
)
