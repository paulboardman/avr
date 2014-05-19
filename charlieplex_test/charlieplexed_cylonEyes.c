/*
 * charlieplex_test.c
 *
 * Running on an ATtiny45.
 *
 * Here we control 6 LEDs through 3 pins (PB0:2).
 *
 * In order to illuminate each LED we do the following:
 *
 * LED1 - PB0 & 2 output PB2 input. PB0 sourcing, PB1 sinking. Pull-up on PB2
 * LED2 - PB0 & 2 output PB2 input. PB1 sourcing, PB0 sinking. Pull-up on PB2
 * LED3 - PB1 & 3 output PB0 input. PB1 sourcing, PB2 sinking. Pull-up on PB0
 * LED4 - PB1 & 3 output PB0 input. PB2 sourcing, PB1 sinking. Pull-up on PB0
 * LED5 - PB0 & 3 output PB1 input. PB0 sourcing, PB2 sinking. Pull-up on PB1
 * LED6 - PB0 & 3 output PB1 input. PB2 sourcing, PB0 sinking. Pull-up on PB2
 *
 * This little ASII diagram shows the wiring and orientation of the 6 LEDs.
 * The hyphens ('-') identify the cathode pins of the LEDs.
 *
 * PB0 ----------------------
 *        |   -      |      |
 *        1   2      |      |
 *        -   |      |      -
 * PB1 --------      5      6
 *        |   -      -      |
 *        3   4      |      |
 *        -   |      |      |
 * PB2 ----------------------
 *
 * Only a single LED can be illuminated at any point in time.
 *
 * Distributed under Creative Commons 3.0 -- Attib & Share Alike
 *
 *  Created on: Feb 27, 2010
 *      Author: PaulBo
 */
#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
	#define F_CPU 1000000UL
#endif

#define DELAY_TIME 50
#define N_LED 6

// see class comments for pin setting explanation
// unused pins are set to input with pull-up resistors activated
struct leds {
	uint8_t mDdrB;
	uint8_t mPortB;
} ledData[] = {
		{0b00011011, 0b11100101},
		{0b00011011, 0b11100110},
		{0b00011110, 0b11100011},
		{0b00011110, 0b11100101},
		{0b00011101, 0b11100011},
		{0b00011101, 0b11100110}
};

int main()
{
	uint8_t i;
	for(;;)
	{
		for(i = 0; i < N_LED - 1; i++)
		{
			DDRB = ledData[i].mDdrB;
			PORTB = ledData[i].mPortB;
			_delay_ms(DELAY_TIME);
		}
		for(i = N_LED - 1;i > 0; i--)
		{
			DDRB = ledData[i].mDdrB;
			PORTB = ledData[i].mPortB;
			_delay_ms(DELAY_TIME);
		}
	}
}
