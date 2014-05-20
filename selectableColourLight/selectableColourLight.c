/*
 * selectableColourLight.c
 *
 * Distributed under Creative Commons 3.0 -- Attib & Share Alike
 *
 * Based on the rgbLedNightLight project.  This code uses the value from a potentiometer
 * to select a colour (RGB) to display.
 *
 *  Created on: Apr 11, 2010
 *      Author: Paul
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifndef F_CPU
    #define F_CPU 8000000UL
#endif

//Hardware definitions
#define RED_LED      PB2
#define GREEN_LED    PB0
#define BLUE_LED     PB1
#define ALL_LEDS    ((1 << RED_LED) | (1 << GREEN_LED) | (1 << BLUE_LED))

#define POT_PIN PB4

#define RED_INDEX   0
#define GREEN_INDEX 1
#define BLUE_INDEX  2

//this is 1023 (10-bit ADC max value)/6 (number of transitions)
#define SCALING_RANGE 170
#define SCALING_FACTOR 255/SCALING_RANGE

unsigned char mRgbBuffer[] = {0,0,0};
volatile unsigned char mRgbValues[] = {0,0,0};

void initAdc()
{
	// Enable ADC
	ADCSRA = (1 << ADEN);

	// Select division factor 64, so we get 8 MHz/64 = 125 kHz ADC clock
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1);

	// Use Vcc as voltage reference so leave REFS2:0 as 000

	//we only need 8-bit precision.  Left adjust the ADC result
	//so that we can read the ADCH register and be done with it.
	//ADMUX |= (1 << ADLAR);
	//actually, lets go with 10 bit precision for 1024 colours (rather than 256)

	// To select ADC2 we set ADMUX register bits MUX3..0=0010
	ADMUX |= (1 << MUX1);
}

void init_timers()
{
    TIMSK = (1 << TOIE0); // enable overflow interrupt
    TCCR0B = (1 << CS00);  // start timer, no prescale

    //enable interrupts
    sei();
}

uint16_t readAdc()
{
	// Start ADC conversion
	ADCSRA |= (1<<ADSC);

	//wait for the conversion to complete
	while(!(ADCSRA & (1 << ADIF)));

	return(ADC);//return the full 10-bit value
}

/**
 * sets all 3 RGB values from the single input value.
 * This is based on the following transitions (as in the RGB Night light project)
 * 1. 255,  0,  0 ->
 * 2. 255,255,  0 ->
 * 3.   0,255,  0 ->
 * 4.   0,255,255 ->
 * 5.   0,  0,255 ->
 * 6. 255,  0,255 -> 1.
 *
 * The 10-bit ADC has a max value of 1023.  So, for the 6 transitions we have
 * 1024/6 = 170.6 values to choose from.
 *
 * We scale the ADC input as follows:
 *
 * ADC value < 170  = transition 1. (increase Green value)
 *           < 342  = 2. (decrease Red)
 *           < 512 = 3. (increase Blue)
 *           < 683 = 4. (decrease Green)
 *           < 854 = 5. (increase Red)
 *           >=854 = 6. (decrease Blue)
 *
 * Then the value for the increment/decremented channel is as follows:
 *
 * For increasing colour values:
 *   (ADC value - ((n - 1) * SCALING_RANGE)  * SCALING_FACTOR
 *   where 'n' is the transition number (1 - 6, see above).
 *
 * For decreasing values we use 255 - the above value.
 */
void setRgbLevels(uint16_t pValue)
{
	if(pValue < SCALING_RANGE)
	{
		mRgbValues[RED_INDEX]   = 255;
		mRgbValues[GREEN_INDEX] = pValue * SCALING_FACTOR;
		mRgbValues[BLUE_INDEX]  = 0;

	}
	else if(pValue < 342) //SCALING_RANGE * 2
	{
		mRgbValues[RED_INDEX]   = 255 - ((pValue - SCALING_RANGE) * SCALING_FACTOR);
		mRgbValues[GREEN_INDEX] = 255;
		mRgbValues[BLUE_INDEX]  = 0;
	}
	else if(pValue < 512) //SCALING_RANGE * 3
	{
		mRgbValues[RED_INDEX]   = 0;
		mRgbValues[GREEN_INDEX] = 255;
		mRgbValues[BLUE_INDEX]  = (pValue - 342) * SCALING_FACTOR;
	}
	else if(pValue < 683)//SCALING_RANGE * 4
	{
		mRgbValues[RED_INDEX]   = 0;
		mRgbValues[GREEN_INDEX] = 255 - ((pValue - 512) * SCALING_FACTOR);
		mRgbValues[BLUE_INDEX]  = 255;
	}
	else if(pValue < 854)//SCALING_RANGE * 5
	{
		mRgbValues[RED_INDEX]   = (pValue - 683) * SCALING_FACTOR;
		mRgbValues[GREEN_INDEX] = 0;
		mRgbValues[BLUE_INDEX]  = 255;
	}
	else
	{
		mRgbValues[RED_INDEX]   = 255;
		mRgbValues[GREEN_INDEX] = 0;
		mRgbValues[BLUE_INDEX]  = 255 - ((pValue - 854) * SCALING_FACTOR);
	}
}

int main(void)
{
	//Set LED pins as output
	DDRB |= ALL_LEDS;

	//Set POT PIN as input
	DDRB &= ~(1 << POT_PIN);

	initAdc();
	init_timers();
	uint16_t mValue = 0;

	for(;;)
	{
		mValue = readAdc();
		setRgbLevels(mValue);
		_delay_ms(10);
	}
	return 0;
}

/*
 * Timer/Counter overflow interrupt. This is called each time
 * the counter overflows (255 counts/cycles).
 */
ISR(TIM0_OVF_vect)
{
    //these static variables maintain state from one call to the next
    static unsigned char sPortBmask = ~ALL_LEDS;
    static unsigned char sCounter = 255;

    //set port pins straight away (no waiting for processing)
    PORTB = sPortBmask;

    //this counter will overflow back to 0 after reaching 255.
    //So we end up adjusting the LED states for every 256 interrupts/overflows.
    if(++sCounter == 0)
    {
    	mRgbBuffer[RED_INDEX] = mRgbValues[RED_INDEX];
    	mRgbBuffer[GREEN_INDEX] = mRgbValues[GREEN_INDEX];
    	mRgbBuffer[BLUE_INDEX] = mRgbValues[BLUE_INDEX];

        //set all pins to low (remember this is a common anode LED)
        sPortBmask &=~ ALL_LEDS;
        sPortBmask |= (1 << POT_PIN);
    }
    //this loop is considered for every overflow interrupt.
    //this is the software PWM.
    if(mRgbBuffer[RED_INDEX]   == sCounter) sPortBmask |= (1 << RED_LED);
    if(mRgbBuffer[GREEN_INDEX] == sCounter) sPortBmask |= (1 << GREEN_LED);
    if(mRgbBuffer[BLUE_INDEX]  == sCounter) sPortBmask |= (1 << BLUE_LED);
}
