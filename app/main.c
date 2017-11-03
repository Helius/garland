#include <avr/io.h>
#include <util/delay.h>


void togglePin(void)
{
	// toggle pin
	PORTB ^= (1 << PIN5);
	// wait for 250ms
	_delay_ms(250);
}


int main(void)
{
	DDRB |= (1 << PIN5);

	while(1)
	{
		togglePin();
	}
}

