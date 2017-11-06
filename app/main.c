#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include <uart.h>
#include <ws2812b.h>

const unsigned long int rainbow[] PROGMEM = {
	0xFF0000,
	0xFF7D00,
	0xFFFF00,
	0x00FF00,
	0x00FFFF,
	0x0000FF,
	0xFF00FF
};

const unsigned char sin_arr[] PROGMEM = {
	0,
	8,
	16,
	24,
	32,
	40,
	48,
	56,
	64,
	72,
	80,
	88,
	95,
	103,
	110,
	117,
	125,
	132,
	139,
	145,
	152,
	158,
	165,
	171,
	177,
	183,
	188,
	194,
	199,
	204,
	209,
	213,
	218,
	222,
	226,
	229,
	233,
	236,
	239,
	242,
	244,
	246,
	248,
	250,
	251,
	252,
	253,
	254,
	254,
	255,
};

unsigned char sin_value(int value, int period)
{
	int t_ind = (value * sizeof(sin_arr) * 2)/period;
	if (t_ind < sizeof(sin_arr)) {
		return pgm_read_byte(&(sin_arr[t_ind/2]));
	}
	return pgm_read_byte(&(sin_arr[sizeof(sin_arr) - t_ind/2]));
}

void togglePin(void)
{
	// toggle pin
	PORTB ^= (1 << PIN5);
	// wait for 250ms
	_delay_ms(250);
}


int main(void)
{
	
	initLEDs();

	uart_init();
	printf("Hi\n\r");


	DDRB |= (1 << PIN5);
	togglePin();

	setMaxBrightness(30);
  
	// delay for enabling bluetooth board power
	// allow to program board via serial port
	_delay_ms(3000);
	DDRD |= 1 << PIN6;
	PORTD |= 1 << PIN6;
/*
	int j = 0;
	int period = 4;
	while(1)
	{
		j++;
		for (int i = 0; i < 50; ++i) {
			setR(i, 0, sin_value((i+j)%period, period));
			setG(i, 0, sin_value((i+j+period/3)%period, period));
//			setB(i, 0, sin_value((i+j+(period*2)/3)%period, period));
		}
		showLEDs();
		_delay_ms(180);
	}
*/
	/*
	while(1)
	{
		
		setRGB(0,0,0);
		switch(rand()%3) {
			case 0:
				setR(0, 0, 0xFF);
				break;
			case 1:
				setG(0, 0, 0xFF);
				break;
			case 2:
				setB(0, 0, 0xFF);
				break;
		}
		for (int i = 49; i > 0; --i) {
			setR(i,0,getR(i-1,0));
			setG(i,0,getG(i-1,0));
			setB(i,0,getB(i-1,0));
		}
		showLEDs();
		_delay_ms(100);
	}
*/
	// shifting rainbow
	unsigned char rainbow_ind = 0;
	while(1)
	{
		setRGB(0, 0, pgm_read_dword(&(rainbow[rainbow_ind%7])));	
		rainbow_ind++;
		for (int i = 49; i > 0; --i) {
			setR(i,0,getR(i-1,0));
			setG(i,0,getG(i-1,0));
			setB(i,0,getB(i-1,0));
		}
		showLEDs();
		_delay_ms(100);
	}
}

