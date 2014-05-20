/*
 *
 * WARNING: THIS IS A WORK IN PROGRESS.
 * CODE MAY BE BROKEN AND BADLY DOCUMENTED.
 *
 * UV_Painter.c
 *
 * Running on an ATtiny45.
 *
 * Here we control 6 LEDs through 3 pins (PB0:2).
 *
 * In order to illuminate each LED we do the following:
 *
 * LED1 - PB0:1 output PB2 & 3 input. PB0 sourcing, PB1 sinking. Pull-up on PB2:3
 * LED2 - PB0:1 output PB2 & 3 input. PB1 sourcing, PB0 sinking. Pull-up on PB2:3
 * LED3 - PB1:2 output PB0 & 3 input. PB1 sourcing, PB2 sinking. Pull-up on PB0 and PB3
 * LED4 - PB1:2 output PB0 & 3 input. PB2 sourcing, PB1 sinking. Pull-up on PB0 and PB3
 * LED5 - PB0 & 2 output PB1 & 3 input. PB0 sourcing, PB2 sinking. Pull-up on PB1 and PB3
 * LED6 - PB0 & 2 output PB1 & 3 input. PB2 sourcing, PB0 sinking. Pull-up on PB2:3
 * LED7 - PB2:3 output PB0:1 input. PB3 sourcing, PB2 sinking. Pull-up on PB0:1
 * LED8 - PB2:3 output PB0:1 input. PB2 sourcing, PB3 sinking. Pull-up on PB0:1
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
 *  Created on: Feb 28, 2010
 *      Author: PaulBo
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#ifndef F_CPU
	#define F_CPU 1000000UL
#endif

#define DELAY_TIME 50
#define N_LED 8
#define N_ELEMENTS 94//number of elements/columns in the message
#define N_BLANKS 250 //number of blanks at end of sequence
#define N_DELAY 2 // ms delay between each LED update

// see class comments for pin setting explanation
// unused pins are set to input with pull-up resistors activated
struct leds {
	uint8_t mDdrB;
	uint8_t mPortB;
} ledData[N_LED] = {
		{0b00010011, 0b11111101}, // LED1
		{0b00010011, 0b11111110}, // LED2
		{0b00010110, 0b11111011}, // LED3
		{0b00010110, 0b11111101}, // LED4
		{0b00010101, 0b11111011}, // LED5
		{0b00010101, 0b11111110}, // LED6
		{0b00011100, 0b11111011}, // LED7
		{0b00011100, 0b11110111}, // LED8
};

const struct leds ledsAllOff = {0b000000, 0b0111111};

const uint8_t mCylonScan[13][N_LED] PROGMEM = {
		{1,0,0,0,0,0,0,0},
		{0,1,0,0,0,0,0,0},
		{0,0,1,0,0,0,0,0},
		{0,0,0,1,0,0,0,0},
		{0,0,0,0,1,0,0,0},
		{0,0,0,0,0,1,0,0},
		{0,0,0,0,0,0,1,0},
		{0,0,0,0,0,0,0,1},
		{0,0,0,0,0,1,0,0},
		{0,0,0,0,1,0,0,0},
		{0,0,0,1,0,0,0,0},
		{0,0,1,0,0,0,0,0},
		{0,1,0,0,0,0,0,0}
};

const uint8_t mFangletronicsChars[94][N_LED] PROGMEM = {
		{1,1,1,1,1,1,1,1},
		{0,0,0,0,1,0,0,1},
		{0,0,0,0,1,0,0,1},
		{0,0,0,0,1,0,0,1},
		{0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,0,0},
		{0,0,0,1,0,0,1,0},
		{0,0,0,1,0,0,0,1},
		{0,0,0,1,0,0,1,0},
		{1,1,1,1,1,1,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,1,0},
		{0,0,0,0,0,1,0,0},
		{0,0,0,0,1,0,0,0},
		{0,0,0,1,0,0,0,0},
		{1,1,1,1,1,1,1,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,1,1,1,1,0,0},
		{0,1,0,0,0,0,1,0},
		{1,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,1},
		{1,0,1,0,0,0,0,1},
		{1,1,1,0,0,0,0,1},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0},
		{1,0,0,0,0,0,0,0},
		{1,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1},
		{1,0,0,0,1,0,0,1},
		{1,0,0,0,1,0,0,1},
		{1,0,0,0,1,0,0,1},
		{1,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1},
		{0,0,0,0,1,0,0,1},
		{0,0,0,0,1,0,0,1},
		{0,0,0,0,1,0,0,1},
		{0,0,0,1,0,1,1,0},
		{0,0,1,0,0,0,0,0},
		{1,1,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,1,1,1,1,1,1,0},
		{1,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,1},
		{0,1,1,1,1,1,1,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,1,0},
		{0,0,0,0,0,1,0,0},
		{0,0,0,0,1,0,0,0},
		{0,0,0,1,0,0,0,0},
		{1,1,1,1,1,1,1,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,1,1,1,1,1,1,0},
		{1,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,0,0,0,1,1,1,0},
		{1,0,0,0,1,0,0,1},
		{1,0,0,0,1,0,0,1},
		{1,0,0,0,1,0,0,1},
		{0,1,1,1,1,0,0,1},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0}
};

const uint8_t mInvadersChars[44][N_LED] PROGMEM = {
		{0,1,1,1,0,0,0,0},
		{0,0,0,1,1,0,0,0},
		{0,1,1,1,1,1,0,1},
		{1,0,1,1,0,1,1,0},
		{1,0,1,1,1,1,0,0},
		{0,0,1,1,1,1,0,0},
		{1,0,1,1,1,1,0,0},
		{1,0,1,1,0,1,1,0},
		{0,1,1,1,1,1,0,1},
		{0,0,0,1,1,0,0,0},
		{0,1,1,1,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,1,1,1,1,0},
		{0,0,1,1,1,0,0,0},
		{1,0,1,1,1,1,0,1},
		{0,1,1,1,0,1,1,0},
		{0,0,1,1,1,1,0,0},
		{0,0,1,1,1,1,0,0},
		{0,0,1,1,1,1,0,0},
		{0,1,1,1,0,1,1,0},
		{1,0,1,1,1,1,0,1},
		{0,0,1,1,1,0,0,0},
		{0,0,0,1,1,1,1,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,0,0,1,1,0,0,0},
		{0,1,0,1,1,1,0,0},
		{1,0,1,1,0,1,1,0},
		{0,1,0,1,1,1,1,1},
		{0,1,0,1,1,1,1,1},
		{1,0,1,1,0,1,1,0},
		{0,1,0,1,1,1,0,0},
		{1,0,0,1,1,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,1,0,1,1,0,0,0},
		{1,0,1,1,1,1,0,0},
		{0,0,0,1,0,1,1,0},
		{0,0,1,1,1,1,1,1},
		{0,0,1,1,1,1,1,1},
		{0,0,0,1,0,1,1,0},
		{1,0,1,1,1,1,0,0},
		{0,1,0,1,1,0,0,0}
};

int main()
{
	uint8_t i, j, k;
	for(i = 0;; i++)
	{
		for(j = 0; j < N_ELEMENTS; j++)
		{
			DDRB = ledsAllOff.mDdrB;
			PORTB = ledsAllOff.mPortB;
			for(k = 0; k < N_LED; k++)
			{
				if(pgm_read_byte(&(mFangletronicsChars[j][k])) == 1)
				{
					DDRB = ledData[k].mDdrB;
					PORTB = ledData[k].mPortB;
				}
				_delay_ms(N_DELAY);
			}
		}
		DDRB = ledsAllOff.mDdrB;
		PORTB = ledsAllOff.mPortB;
		for(j = 0; j < N_BLANKS; j++)
		{
			_delay_ms(N_DELAY);
		}
	}
}
