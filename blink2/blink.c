#include <avr/io.h>
#include <avr/delay.h>

//LED is wired into pin 7 (PB2)
#define LED PB2

int main(void){
	//set data direction register for pin 7 to output
	DDRB |= _BV(DDB2);

	//infinite loop
	while (1) {
		//turn on the LED
		PORTB |= _BV(LED);
		//wait for 1/4 of a second
		_delay_ms(250);
		//turn off the LED
		PORTB &= ~_BV(LED);
		//wait for 1/4 second
		_delay_ms(250);
	}
}
