/*
 * multi_rgb.c
 *
 * Cycle 3 RGB LEDs through colour spectrum.
 *
 * Current implementation has out-of phase cycles for
 * all 3 RGB LEDs.
 *
 * ATmega8 Internal RC oscillator clock source at 8MHz
 * i.e. fuse bytes are set to allow ~8MHz operation.
 *
 * Distributed under Creative Commons 3.0 -- Attib & Share Alike
 *
 *  Created on: Feb 20, 2010
 *      Author: PaulBo
 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define LED_RED_0 PD7
#define LED_GREEN_0 PD6
#define LED_BLUE_0 PD5
#define LED_RED_1 PD4
#define LED_GREEN_1 PD3
#define LED_BLUE_1 PD2
#define LED_RED_2 PB1
#define LED_GREEN_2 PB2
#define LED_BLUE_2 PB3
#define OUTPUT_MASK_0 (1 << LED_RED_0) | (1 << LED_GREEN_0) | (1 << LED_BLUE_0)
#define OUTPUT_MASK_1 (1 << LED_RED_1) | (1 << LED_GREEN_1) | (1 << LED_BLUE_1)
#define OUTPUT_MASK_2 (1 << LED_RED_2) | (1 << LED_GREEN_2) | (1 << LED_BLUE_2)

#define RED_INDEX   0
#define GREEN_INDEX 1
#define BLUE_INDEX  2
#define RED_MAX 200
#define GREEN_MAX 200
#define BLUE_MAX 200

//define the processor speed (if it's not already been defined by the compiler)
#ifndef F_CPU
	#define F_CPU 8000000UL
#endif

//Cycle States
#define RedToYellow     0
#define YellowToGreen   1
#define GreenToCyan     2
#define CyanToBlue      3
#define BlueToMagenta    4
#define MagentaToRed     5

//set initial led states and values
volatile unsigned char mRgbBuffer[3][3];
unsigned char mRgbValues[3][3] = {{255,0,0}, {0,255,0}, {0,0,255}};
unsigned char mState[3] = {0, 2, 4};
uint8_t i;

// timer0 is used for the software PWM
void initTimers()
{
	//enable timer/counter0 overflow interrupt
	TIMSK |= (1 << TOIE0);

	//start timer0, no prescale
	TCCR0 = (1 << CS00);

	//enable interrupts
	sei();
}


void rgbCycle(uint8_t pLed){
	//Go one step through a state machine that fades through the rainbow
	//n sets the step increment
	//255%n must equal 0
	switch (mState[pLed]) {
	case RedToYellow:
		mRgbValues[pLed][GREEN_INDEX]++;
		if (mRgbValues[pLed][GREEN_INDEX] == GREEN_MAX)
			mState[pLed]++;
		break;
	case YellowToGreen:
		mRgbValues[pLed][RED_INDEX]--;
		if (mRgbValues[pLed][RED_INDEX] == 0)
			mState[pLed]++;
		break;
	case GreenToCyan:
		mRgbValues[pLed][BLUE_INDEX]++;
		if (mRgbValues[pLed][BLUE_INDEX] == BLUE_MAX)
			mState[pLed]++;
		break;
	case CyanToBlue:
		mRgbValues[pLed][GREEN_INDEX]--;
		if (mRgbValues[pLed][GREEN_INDEX] == 0)
			mState[pLed]++;
		break;
	case BlueToMagenta:
		mRgbValues[pLed][RED_INDEX]++;
		if (mRgbValues[pLed][RED_INDEX] == RED_MAX)
			mState[pLed]++;
		break;
	case MagentaToRed:
		mRgbValues[pLed][BLUE_INDEX]--;
		if (mRgbValues[pLed][BLUE_INDEX] == 0)
			mState[pLed]++;
		break;
	}

	//state should never advance beyond 5.
	//It wraps back to 0 when we reach 6
	mState[pLed] %= 6;
}

int main(void)
{
	DDRD |= (OUTPUT_MASK_0 | OUTPUT_MASK_1);//led set as output
	DDRB |= OUTPUT_MASK_2;
	initTimers();

	while (1) {
			rgbCycle(0);
			_delay_ms(5);
			rgbCycle(1);
			_delay_ms(5);
			rgbCycle(2);
			_delay_ms(5);
	}
}

/*
 * Timer/Counter overflow interrupt (timer0). This is called each time
 * the counter overflows (255 counts/cycles).
 */
ISR(TIMER0_OVF_vect)
{
	//static variables maintain state from one call to the next
	static unsigned char sPortDmask = OUTPUT_MASK_0 | OUTPUT_MASK_1;
	static unsigned char sPortBmask = OUTPUT_MASK_2;
	static unsigned char sCounter = 255;

	//set port pins straight away (no waiting for processing)
	PORTD = sPortDmask;
	PORTB = sPortBmask;

	//this counter will overflow back to 0 after reaching 255.
	//So we end up adjusting the LED states for every 256 overflows.
	if(++sCounter == 0)
	{
		for(i = 0; i < 3; i++)
		{
			mRgbBuffer[i][0] = mRgbValues[i][0];
			mRgbBuffer[i][1] = mRgbValues[i][1];
			mRgbBuffer[i][2] = mRgbValues[i][2];
		}
		//set all pins to high
		sPortDmask = OUTPUT_MASK_0 | OUTPUT_MASK_1;
		sPortBmask = OUTPUT_MASK_2;
	}
	//this loop is considered for every overflow interrupt.
	//this is the software PWM.
	if(mRgbBuffer[0][0] == sCounter) sPortDmask &= ~(1 << LED_RED_0);
	if(mRgbBuffer[0][1] == sCounter) sPortDmask &= ~(1 << LED_GREEN_0);
	if(mRgbBuffer[0][2] == sCounter) sPortDmask &= ~(1 << LED_BLUE_0);
	if(mRgbBuffer[1][0] == sCounter) sPortDmask &= ~(1 << LED_RED_1);
	if(mRgbBuffer[1][1] == sCounter) sPortDmask &= ~(1 << LED_GREEN_1);
	if(mRgbBuffer[1][2] == sCounter) sPortDmask &= ~(1 << LED_BLUE_1);
	if(mRgbBuffer[2][0] == sCounter) sPortBmask &= ~(1 << LED_RED_2);
	if(mRgbBuffer[2][1] == sCounter) sPortBmask &= ~(1 << LED_GREEN_2);
	if(mRgbBuffer[2][2] == sCounter) sPortBmask &= ~(1 << LED_BLUE_2);
}
