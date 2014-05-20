/*
 * IrLedPulse.c
 *
 * Distributed under Creative Commons 3.0 -- Attib & Share Alike
 *
 * Created on: Dec 26, 2009
 *      Author: Paul
 */
#include <avr/io.h>
#include <avr/delay.h>

#ifndef F_CPU
	#define F_CPU 1000000UL
#endif


void pulseIr()
{
	// Use Timer 0 to pulse the IR LED at 38KHz
	//
	// We pulse the IR emitter for only 170 microseconds.  We do this to save battery power.
	//   The minimum number of 38KHz pulses that the IR detector needs to detect IR is 6.
	//   170 microseconds gives more than 6 periods of 38KHz.

	// Start up Timer0 in CTC (clear timer on compare) mode at about 38KHz to drive the IR emitter on output OC0A:
	//   8-bit Timer0 OC0A (PB0, pin 5) is set up for CTC mode, toggling output on each compare
	//   Fclk = Clock = 1MHz
	//   Prescale = 1/8
	//   OCR0A = 104 (this is the compare value)
	//   F = Fclk / (2 * Prescale * (1 + OCR0A) ) = 38KHz
	//   F = 1,000,000 / (2 * 1/8 * 105) = 38095 Hz
	TCCR0A = 0| (1 << COM0A0) | (1 << WGM01); // COM0A0=1 to toggle OC0A on Compare Match

	TCCR0B = 0 | (1 << CS01);	// CS01=1 for divide by 8 prescaler (this starts Timer0)
	OCR0A = 104;  // to output 38,095.2KHz on OC0A (PB0, pin 5)

	// keep the IR going at 38KHz for 170 microseconds (which is slightly more than 6 periods of 38KHz)
	_delay_us(170);   // delay 170 microseconds

	// turn off Timer0 to stop 38KHz pulsing of IR
	TCCR0B = 0b00000000;  // CS02:CS00=000 to stop Timer0 (turn off IR emitter)
	TCCR0A = 0b00000000;  // COM0A1:0=00 to disconnect OC0A from PB0 (pin 5)
}

int main(void)
{
	DDRB = 0b00000001;//set PB0 to output
	PORTB = 0xFF; // all PORTB output pins high (LED off).
	while(1)
	{
		pulseIr();
		_delay_us(400);
		//nothing to do here.  Ir pulse is via Timer 0
	}
}
