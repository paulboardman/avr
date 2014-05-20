/*
 * cylon_eyes.c
 *
 * Simple scan back and forth for 5 LEDs connected as
 * defined below (see led defs).
 *
 * Distributed under Creative Commons 3.0 -- Attib & Share Alike
 *
 *  Created on: Feb 22, 2010
 *      Author: Paul
 */

#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
	#define F_CPU 1000000UL
#endif

#define LED1 PB4
#define LED2 PB3
#define LED3 PB2
#define LED4 PB1
#define LED5 PB0
#define DELAY_TIME 150

uint8_t mLeds[5] = {LED1, LED2, LED3, LED4, LED5};

int main()
{
	uint8_t i;
	DDRB |= (1 << LED1) | (1 << LED2) | (1 << LED3) | (1 << LED4) | (1 << LED5);
	for(;;)
	{
		for(i = 0; i < 4; i++)
		{
			PORTB = 0;
			PORTB = (1 << mLeds[i]);
			_delay_ms(DELAY_TIME);
		}
		for(;i > 0; i--)
		{
			PORTB = 0;
			PORTB = (1 << mLeds[i]);
			_delay_ms(DELAY_TIME);
		}
	}
}
