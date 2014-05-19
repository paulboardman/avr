/*
 * pwm_traffic_lights_US.c
 *
 *  Created on: Jun 23, 2011
 *      Author: Paul
 *
 *  Uses software PWM to control 3 LED's (RED, YELLOW, GREEN)
 *  to mimic a simple, US style, traffic light.
 *
 *  This was written to control a kids toy traffic light. See:
 *  http://www.fangletronics.com/2009/12/duplo-traffic-lights.html
 *  http://www.fangletronics.com/2009/08/toy-traffic-lights.html
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifndef F_CPU
    #define F_CPU 8000000UL
#endif

//Hardware definitions
#define RED_LED      PB2
#define YELLOW_LED   PB3
#define GREEN_LED    PB4
#define ALL_LEDS    ((1 << RED_LED) | (1 << GREEN_LED) | (1 << YELLOW_LED))

//Maximum value for led brightness
#define R_MAX 175
#define G_MAX 150
#define Y_MAX 100

#define RED_INDEX   0
#define GREEN_INDEX 1
#define YELLOW_INDEX  2

#define RED_DELAY 10
#define YELLOW_DELAY 3
#define GREEN_DELAY 15

volatile unsigned char mLedBuffer[] = {0,0,0};
unsigned char mLedValues[] = {R_MAX,0,0};

void init_timers()
{
    TIMSK  = (1 << TOIE0);// enable overflow interrupt
    TCCR0B = (1 << CS00); // start timer, no prescale

    //enable interrupts
    sei();
}


uint8_t i = 0;
uint8_t j = 0;
void delay_seconds(uint8_t pSecs)
{
  for(i = 0; i < pSecs; i++)
  {
      for(j = 0; j < 4; j++)
          _delay_ms(250);
  }
}

void resetLedValues()
{
	mLedValues[RED_INDEX] = 0;
	mLedValues[GREEN_INDEX] = 0;
	mLedValues[YELLOW_INDEX] = 0;
}

int main(void){
	//Set LED pins to output
	DDRB |= ALL_LEDS;

	init_timers();

	while (1) {
		resetLedValues();
		mLedValues[RED_INDEX] = R_MAX;
		delay_seconds(RED_DELAY);

		resetLedValues();
		mLedValues[GREEN_INDEX] = G_MAX;
		delay_seconds(GREEN_DELAY);

		resetLedValues();
		mLedValues[YELLOW_INDEX] = Y_MAX;
		delay_seconds(YELLOW_DELAY);
	}
	return 0;
}

/*
 * Timer/Counter overflow interrupt. This is called each time
 * the counter overflows (255 counts/cycles).
 */
ISR(TIM0_OVF_vect)
{
    //static variables maintain state from one call to the next
	// start with all LED's off
    static unsigned char sPortBmask = 0b00000000;
    static unsigned char sCounter = 255;

    //set port pins straight away (no waiting for processing)
    PORTB = sPortBmask;

    //this counter will overflow back to 0 after reaching 255.
    //So we end up adjusting the LED states for every 256 interrupts/overflows.
    if(++sCounter == 0)
    {
    	mLedBuffer[RED_INDEX]    = mLedValues[RED_INDEX];
    	mLedBuffer[GREEN_INDEX]  = mLedValues[GREEN_INDEX];
    	mLedBuffer[YELLOW_INDEX] = mLedValues[YELLOW_INDEX];

        //set all pins to high (all LEDs on)
        sPortBmask |= ALL_LEDS;
    }
    //this loop is considered for every overflow interrupt.
    //this is the software PWM.
    //when an LED's value is reached the LED turned off for the remainder of the PWM cycle
    if(mLedBuffer[RED_INDEX]   == sCounter)   sPortBmask &= ~(1 << RED_LED);
    if(mLedBuffer[GREEN_INDEX] == sCounter)   sPortBmask &= ~(1 << GREEN_LED);
    if(mLedBuffer[YELLOW_INDEX]  == sCounter) sPortBmask &= ~(1 << YELLOW_LED);
}
