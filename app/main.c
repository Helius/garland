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

// 1/4 of sin wave
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


#define MODE_RAINBOW        0
#define MODE_CMD            1
#define MODE_RANDOM_FORWARD 2

unsigned char width = 50;
unsigned char mode = MODE_RAINBOW;
unsigned char loop_delay = 50; // x2 ms



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

void shift(int forvard) {
	if (forvard) {
		for (int i = width-1; i > 0; --i) {
			setR(i,0,getR(i-1,0));
			setG(i,0,getG(i-1,0));
			setB(i,0,getB(i-1,0));
		}
	} else {
		for (int i = 0; i < width-1; ++i) {
			setR(i,0,getR(i+1,0));
			setG(i,0,getG(i+1,0));
			setB(i,0,getB(i+1,0));
		}
	}
}

#define S_IDLE 0
#define S_DATA 1

#define C_BRIGHT   0x0 // value 8b[value 0..255]
#define C_MODE     0x1 // value 8b[mode 0..x]
#define C_SPEED    0x2 // value 8b[loop delay ms]
#define C_SET_LED  0x3 // value 8b[index] 8b[R] 8b[G] 8b[B]
#define C_SET_ALL  0x4 // value 8b[R] 8b[G] 8b[B]
#define C_WIDTH    0x5 // value 8b[width]

void run_command (unsigned char cmd, unsigned char * value) {
	switch (cmd) {

		case C_BRIGHT:
//			printf("cmd:bright, value 0x%x\n\r", value[0]);
			setMaxBrightness(value[0]);
			break;

		case C_MODE:
//			printf("cmd:mode, value 0x%x\n\r", value[0]);
			mode = value[0];
			break;

		case C_SPEED:
//			printf("cmd:speed, value 0x%x\n\r", value[0]);
			loop_delay = value[0];
			break;

		case C_WIDTH:
//			printf("cmd:width, value 0x%x\n\r", value[0]);
			width = value[0];
			break;

		case C_SET_LED:
			mode = MODE_CMD;
	//		printf("cmd:set led, ind 0x%x, value 0x%x%x%x\n\r", value[0], value[1], value[2], value[3]);
			setR(value[0], 0, value[1]);
			setG(value[0], 0, value[2]);
			setB(value[0], 0, value[3]);
			break;
		
		case C_SET_ALL:
			mode = MODE_CMD;
//			printf("cmd:set ALL, value 0x%x%x%x\n\r", value[0], value[1], value[2]);
		  
			for (unsigned char i = 0; i < width; ++i) {
				setR(i, 0, value[0]);
				setG(i, 0, value[1]);
				setB(i, 0, value[2]);
			}
			break;

		default:
			printf ("unk cmd: 0x%x\n\r", value[0]);
			break;
	}
}


void uart_rx_handler(unsigned char ch)
{
	static unsigned char state = 0;
	static unsigned char cmd = 0;
	static unsigned char value[8];
	static unsigned char will_receive = 0;
	static unsigned char index = 0;

	switch(state) {

		case S_IDLE:
			cmd = ch;
			index = 0;
			if (cmd == C_SET_LED) {
				will_receive = 4;
			} else if (cmd == C_SET_ALL) {
				will_receive = 3;
			} else {
				will_receive = 1;
			}
			state = S_DATA;
			//printf("\n\rVVVV got cmd VVVV need %d more\n\r", will_receive);
			break;

		case S_DATA:
			//printf("0x%x ", ch);
			value[index++] = ch;
			if (index == will_receive) {
				//printf("\n\r");
				run_command(cmd, value);
				state = S_IDLE;
			}
			break;
	}
}

int main(void)
{
	
	initLEDs();

	uart_init();
  set_receive_interrupt_handler(&uart_rx_handler);
	printf("Hi from helius garland!\n\r");
	


	DDRB |= (1 << PIN5);
	togglePin();

	setMaxBrightness(30);
  
	// delay for enabling bluetooth board power
	// allow to program board via serial port
	_delay_ms(3000);
	DDRD |= 1 << PIN6;
	PORTD |= 1 << PIN6;
	sei();

	while(1)
	{
		switch (mode) {

			case MODE_RAINBOW: 
				{
					static unsigned char rainbow_ind = 0;
					shift(1);
					setRGB(0, 0, pgm_read_dword(&(rainbow[rainbow_ind%7])));	
					rainbow_ind++;
				}
				break;

			case MODE_CMD:
				// do nothing, do cmd from serial port
				break;

			case MODE_RANDOM_FORWARD:
				shift(1);
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
				break;
			
			default:
				printf("unknow mode");
				mode = MODE_RAINBOW;
				break;
		}
		
		showLEDs();
		
		for (int i = 0; i < loop_delay; ++i) {
			_delay_ms(2);
		}
	}
}

