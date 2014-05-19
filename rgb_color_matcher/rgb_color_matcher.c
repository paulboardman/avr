/*
 * ATmega8 Internal RC oscillator clock source at 8MHz
 * i.e. fuse bytes are set to allow ~8MHz operation.
 *
 * Distributed under Creative Commons 3.0 -- Attib & Share Alike
 *
 * Created on: Feb 15, 2010
 *     Author: PaulBo
 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <math.h> //for square root function

#define LED_RED_AUTO PD5
#define LED_GREEN_AUTO PD6
#define LED_BLUE_AUTO PD7
#define LED_RED_USER PD2
#define LED_GREEN_USER PD3
#define LED_BLUE_USER PD4
#define OUTPUT_MASK_AUTO (1 << LED_RED_AUTO) | (1 << LED_GREEN_AUTO) | (1 << LED_BLUE_AUTO)
#define OUTPUT_MASK_USER (1 << LED_RED_USER) | (1 << LED_GREEN_USER) | (1 << LED_BLUE_USER)
#define SPEAKER_PIN PB3
#define RED_POT PC0
#define GREEN_POT PC1
#define BLUE_POT PC2
#define INPUT_MASK ((1 << RED_POT) | (1 << GREEN_POT) | (1 << BLUE_POT))

//define the processor speed (if it's not already been defined by the compiler)
#ifndef F_CPU
	#define F_CPU 8000000UL
#endif

//some constants
#define DELTA 50
#define SINGLE_DIFF_LIMIT 20
#define MIN_RGB_LEVEL 50

uint8_t mUserLedLevels[3] = {0, 0, 0};
uint8_t mAutoLedLevels[3] = {0, 0, 0};
uint8_t mBuffer[6];
uint8_t i;

uint8_t readAdc(uint8_t pChannel)
{
	//clear the previous channel selection
	ADMUX &= 0b11111000;

	//set the new channel
	ADMUX |= pChannel;

	// Start ADC conversion
	ADCSRA |= (1<<ADSC);

	while(!(ADCSRA & (1 << ADIF)));

	return(255 - ADCH);
}

// timer0 is used for the software PWM
// timer2 is used for controlling the speaker (via hardware PWM)
void initTimers()
{
	//enable timer/counter0 overflow interrupt
	TIMSK |= (1 << TOIE0);

	//start timer0, no prescale
	TCCR0 = (1 << CS00);

	//start timer2, 256 prescale
	TCCR2 = (1 << CS22) | (1 << CS21);

	//set CTC mode for timer2 PWM
	TCCR2 |= (1 << WGM21);

	//set toggle logical level on each compare match
	//this allows waveform generation in CTC mode
	TCCR2 |= (1 << COM20);

	//gives frequency of ((8MHz)/256)/255) = 122.55 Hz
	OCR2 = 255;

	//enable interrupts
	sei();
}

void initAdc()
{
	// Enable ADC
	ADCSRA = (1 << ADEN);

	// Select divider factor 64, so we get 8 MHz/64 = 125 kHz ADC clock
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1);

	// Use Vcc as voltage reference
	ADMUX |= (1 << REFS0);

	//we only need 8-bit precision.  Left adjust the ADC result
	//so that we can read the ADCH register and be done with it.
	ADMUX |= (1 << ADLAR);
}

// randomly assign values to the RGB LEDs.
// we require a min value so that we aren't
// playing with v.dark colours
void setAutoLeds()
{
	uint8_t vTotal = 0;
	while(vTotal < MIN_RGB_LEVEL)
	{
		vTotal = 0;
		for(i = 0; i < 3; i++)
		{
			mAutoLedLevels[i] = (uint8_t)(rand()/112);
			vTotal += mAutoLedLevels[i];
		}
	}
}

/*
 * Use a variable stored in EEPROM to ensure the random color
 * sequence changes from one game to the next.
 */
void initRand()
{
	uint8_t vSeed = eeprom_read_word(0); // load last stored seed
	srand(++vSeed); // increment and use value as seed
	eeprom_write_word(0, vSeed); //store the new seed for next time
}

/*
 * Flash the LEDs on a successful match.  We also
 * modulate the speaker frequency.
 */
void flashAndReset()
{
	cli();
	for(i=0;i<5;i++)
	{
		//modulate speaker frequency on success
		OCR2 = i * 50;

		//flash the LEDs
		PORTD |= (OUTPUT_MASK_AUTO | OUTPUT_MASK_USER);

		_delay_ms(100);
		PORTD &=~ (OUTPUT_MASK_AUTO | OUTPUT_MASK_USER);
		_delay_ms(100);
	}
	setAutoLeds();
	sei();
}

/*
 * Compare the RGB vectors of the two RGB LEDs.
 * If they're within kDelta then flash and reset
 * to a new value.
 *
 * We modulate the speaker frequency here as well
 * to give an audio queue for the 'player'.  The
 * speaker can be wired up to a difficulty switch
 * to turn it off (outside of software).
 */
void compareValues()
{
	uint16_t vDistance = 0;
	uint16_t vDiff;
	// variables are supposed to be automatically initialized to 0
	// but this didn't work unless I explicitly initialized to 0
	uint8_t vFail = 0;
	for(i=0;i<3;i++)
	{
		vDiff = pow((mUserLedLevels[i] - mAutoLedLevels[i]),2);
		vDistance += vDiff;
		if(vDiff > SINGLE_DIFF_LIMIT)
		{
			vFail = 1;
		}
	}
	vDistance = sqrt(vDistance);

	//set the speaker frequency
	OCR2 = (uint8_t)((vDistance + 1)/2);

	if(!vFail)
	{
		flashAndReset();
	}
}

int main(void)
{
	DDRD |= (OUTPUT_MASK_AUTO | OUTPUT_MASK_USER);//led set as output
	DDRC &=~ INPUT_MASK;//set potentiometer pin for input
	DDRB |= (1 << SPEAKER_PIN); //speaker output
	initTimers();
	initAdc();
	initRand();
	setAutoLeds();
	//flashAndReset();
	while (1)
	{
		mUserLedLevels[0] = readAdc(RED_POT);
		mUserLedLevels[1] = readAdc(GREEN_POT);
		mUserLedLevels[2] = readAdc(BLUE_POT);
		_delay_ms(10);
		compareValues();
	}
}

/*
 * Timer/Counter overflow interrupt (timer0). This is called each time
 * the counter overflows (255 counts/cycles).
 */
ISR(TIMER0_OVF_vect)
{
	//static variables maintain state from one call to the next
	static unsigned char sPortDmask = OUTPUT_MASK_AUTO | OUTPUT_MASK_USER;
	static unsigned char sCounter = 255;

	//set port pins straight away (no waiting for processing)
	PORTD = sPortDmask;

	//this counter will overflow back to 0 after reaching 255.
	//So we end up adjusting the LED states for every 256 overflows.
	if(++sCounter == 0)
	{
		for(i = 0; i < 3; i++)
		{
			mBuffer[i] = mUserLedLevels[i];
			mBuffer[i + 3] = mAutoLedLevels[i];
		}
		//set all pins to high
		sPortDmask = OUTPUT_MASK_AUTO | OUTPUT_MASK_USER;
	}
	//this loop is considered for every overflow interrupt.
	//this is the software PWM.
	if(mBuffer[0] == sCounter) sPortDmask &= ~(1 << LED_RED_USER);
	if(mBuffer[1] == sCounter) sPortDmask &= ~(1 << LED_GREEN_USER);
	if(mBuffer[2] == sCounter) sPortDmask &= ~(1 << LED_BLUE_USER);
	if(mBuffer[3] == sCounter) sPortDmask &= ~(1 << LED_RED_AUTO);
	if(mBuffer[4] == sCounter) sPortDmask &= ~(1 << LED_GREEN_AUTO);
	if(mBuffer[5] == sCounter) sPortDmask &= ~(1 << LED_BLUE_AUTO);
}
