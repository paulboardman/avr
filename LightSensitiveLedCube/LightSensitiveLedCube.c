/*
 * LightSensitiveLedCube.c
 *
 * Warning: this is incomplete.  Light sensitivity not yet implemented.
 *
 *  Created on: Feb 12, 2010
 *      Author: PaulBo
 */
#include <avr/io.h>
#include <stdlib.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

//define the processor speed (if it's not already been defined by the compiler)
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

//define the cathode pins
#define LAYER1 PC0
#define LAYER2 PC1
#define LAYER3 PC2

//define the column pins
#define COLUMN1 PD3
#define COLUMN2 PD1
#define COLUMN3 PB3
#define COLUMN4 PD4
#define COLUMN5 PD6
#define COLUMN6 PB2
#define COLUMN7 PD0
#define COLUMN8 PD7
#define COLUMN9 PB1
#define N_COLS 9

#define ALL_PORTB_LEDS ((1 << COLUMN3) | (1 << COLUMN6) | (1 << COLUMN9))
#define ALL_PORTD_LEDS ((1 << COLUMN1) | (1 << COLUMN2) | (1 << COLUMN4) | (1 << COLUMN5) | (1 << COLUMN7) | (1 << COLUMN8))

#define CAL_ADDR 2

//store the layers in an array for easy access
uint8_t kLayerConfig[] = { LAYER1, LAYER2, LAYER3 };

//this is an array of pointers.  This stores the
//port which each LED is attached to.
//e.g. LED1/COLUMN1 is attached to PORTD
uint8_t *mPorts[9] = {
		(uint8_t*)&PORTD,
		(uint8_t*)&PORTD,
		(uint8_t*)&PORTB,
		(uint8_t*)&PORTD,
		(uint8_t*)&PORTD,
		(uint8_t*)&PORTB,
		(uint8_t*)&PORTD,
		(uint8_t*)&PORTD,
		(uint8_t*)&PORTB
};

struct ledData {
	uint8_t mLayer;
	uint8_t mColumn;
	uint8_t mBrightness;
} mLeds[3][9] = {
	{
		{	LAYER1, COLUMN1, 0},
		{	LAYER1, COLUMN2, 0},
		{	LAYER1, COLUMN3, 0},
		{	LAYER1, COLUMN4, 0},
		{	LAYER1, COLUMN5, 0},
		{	LAYER1, COLUMN6, 0},
		{	LAYER1, COLUMN7, 0},
		{	LAYER1, COLUMN8, 0},
		{	LAYER1, COLUMN9, 0}
	},
	{
		{	LAYER2, COLUMN1, 125},
		{	LAYER2, COLUMN2, 125},
		{	LAYER2, COLUMN3, 125},
		{	LAYER2, COLUMN4, 125},
		{	LAYER2, COLUMN5, 125},
		{	LAYER2, COLUMN6, 125},
		{	LAYER2, COLUMN7, 125},
		{	LAYER2, COLUMN8, 125},
		{	LAYER2, COLUMN9, 125}
	},
	{
		{	LAYER3, COLUMN1, 200},
		{	LAYER3, COLUMN2, 200},
		{	LAYER3, COLUMN3, 200},
		{	LAYER3, COLUMN4, 200},
		{	LAYER3, COLUMN5, 200},
		{	LAYER3, COLUMN6, 200},
		{	LAYER3, COLUMN7, 200},
		{	LAYER3, COLUMN8, 200},
		{	LAYER3, COLUMN9, 200}
	}
};

//TODO this requires having some of the pins on PORTC!
int getLight(struct ledData *pLedStruct, int pColumnIndex)
{
    int val;
    //set up measuring
    ADMUX |= (1 << REFS0) | (1 << MUX1) | (1 << ADLAR); //measure pb4 using internal ref
    ADCSRA |= (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0);//enable adc, max prescaler

    //kill led2
//    DDRB |= (1 << LED1)  | (1 << LED2);
//    PORTB &=~ (1 << LED2);

    //kill remaining charge in led
    *mPorts[pColumnIndex] &=~ (1 << (*pLedStruct).mColumn);
    _delay_ms(50);

    //let led generate some voltage
//TODO    DDRB &=~ (1 << LED1);
    _delay_ms(50);

    ADCSRA |= (1 << ADSC); //go do adc
    while(ADCSRA & (1 << ADSC)) ; //wait till adc is done

    val = ADC;
    ADCSRA = 0; //disable adc
//    DDRB |= (1 << LED1); //re-enable led
    return val;
}

//void calibrate()
//{
//    //Measure current light level and store in EEPROM
//    //Wait for the level to stabilize first.
//    eeprom_write_word(CAL_ADDR, getLight(mLeds[0][0], 0));
//
//    DDRB |= (1 << CALBRATION_LED);
//    PORTB |= (1 << CALBRATION_LED);
//    while(( PINB & (1 << CALEN)) == 0);
////TODO sort out what you're doing here
//}

void init_timers()
{
	//enable timer/counter0 overflow interrupt
	TIMSK |= (1 << TOIE0);

	//start timer0, no prescale
	//TODO adjusting the prescaler up from no-prescale has
	//positive performance impacts.
	//no prescale resulted in lots of flicker
	TCCR0 = (1 << CS00);
    //enable interrupts
    sei();
}

void updateLEDs()
{
	uint8_t i,j;
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 9; j++)
		{
			mLeds[i][j].mBrightness++;
		}
	}
}

void init_leds()
{
	uint8_t i,j;
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 9; j++)
		{
			mLeds[i][j].mBrightness = rand()/113;
		}
	}
}

int main(void)
{
	DDRB = 0xFF; // all set to output
	DDRD = 0xFF; // all set to output
	DDRC = 0xFF; // cathodes set as output

	//swich off all layers by applying voltage to the cathode pins thereby preventing current flow
	PORTC |= (1 << LAYER1) | (1 << LAYER2) | (1 << LAYER3);

	//initialize the timers (in this case just timer0)
	init_timers();
	init_leds();
	while (1)
	{
		//all the work is done in the interrupt service routine
		_delay_ms(5);
		updateLEDs();
	}
	return 1;
}

/*
 * Timer/Counter overflow interrupt. This is called each time
 * the counter overflows (255 counts/cycles).
 */
ISR(TIMER0_OVF_vect)
{
	uint8_t i;
    //static variables maintain state from one call to the next
    static unsigned char sPortBmask = ALL_PORTB_LEDS;
    static unsigned char sPortDmask = ALL_PORTD_LEDS;
    static unsigned char sCounter = 255;
    static unsigned char sLayer = 2;
    volatile static unsigned char sBrightness[9];

    //set port pins straight away (no waiting for processing)
    PORTB = sPortBmask;
    PORTD = sPortDmask;

    //this counter will overflow back to 0 after reaching 255.
    //So we end up adjusting the LED states for every 256 * nLayers overflows.
    if(++sCounter == 0)
    {
		//set all pins to high
		sPortBmask = ALL_PORTB_LEDS;
		sPortDmask = ALL_PORTD_LEDS;

		//inactivate this layer by applying voltage to the cathode
		PORTC |= (1 << kLayerConfig[sLayer]);

    	//increment the layer but wrap back to 0 at the max number of layers
    	++sLayer; //this is evaluated to a number which is why the modulo happens separately
    	sLayer %= 3;
    	for(i = 0; i < 9; i++)
    	{
    		sBrightness[i] = mLeds[sLayer][i].mBrightness;
    	}
    	//activate the next layer by allowing current to sink into the pin
    	PORTC &=~ (1 << kLayerConfig[sLayer]);
    }
    //this is the software PWM.
    if(sBrightness[0] == sCounter)
    	sPortDmask &= ~(1 << COLUMN1);
    if(sBrightness[1] == sCounter)
    	sPortDmask &= ~(1 << COLUMN2);
    if(sBrightness[2] == sCounter)
    	sPortBmask &= ~(1 << COLUMN3);
    if(sBrightness[3] == sCounter)
    	sPortDmask &= ~(1 << COLUMN4);
    if(sBrightness[4] == sCounter)
    	sPortDmask &= ~(1 << COLUMN5);
    if(sBrightness[5] == sCounter)
    	sPortBmask &= ~(1 << COLUMN6);
    if(sBrightness[6] == sCounter)
    	sPortDmask &= ~(1 << COLUMN7);
    if(sBrightness[7] == sCounter)
    	sPortDmask &= ~(1 << COLUMN8);
    if(sBrightness[8] == sCounter)
    	sPortBmask &= ~(1 << COLUMN9);
}
