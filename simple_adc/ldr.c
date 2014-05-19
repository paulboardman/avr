#include <avr/io.h>
#include <util/delay.h>
#define OUTPUT_LED PB2 // pin 6 (attiny) pin 16 atmega
#define OUTPUT_PWM PB3 // pin 17 (atmega)
#define OUTPUT_PORT DDRB
#define POT PC0 //adc0
#define PWM_MIN 0
#define PWM_MAX 225

uint8_t i;

uint8_t readAdc()
{
	// Start ADC conversion
	ADCSRA |= (1<<ADSC);

	while(!(ADCSRA & (1 << ADIF)));

	return(ADCH);
}

void initTimers()
{
	// 8-bit fast PWM
	TCCR2 |= (1 << WGM20) | (1 << WGM21);

	// Clear OC2 on Compare Match
	// Set OC2 at BOTTOM (non-inverting mode)
	TCCR2 |= (1 << COM21);

	// Set prescaler to 8
	// 1 MHz / 8*256 = ~490 Hz PWM frequency
	TCCR2 |= (1 << CS21);

	OCR2 = 0;
}

void initAdc()
{
	// Enable ADC
	ADCSRA = (1 << ADEN);

	// Select divider factor 8, so we get 1 MHz/8 = 125 kHz ADC clock
	ADCSRA |= (1<<ADPS1) | (1<<ADPS0);

	// To select ADC0 we just leave ADMUX register bits MUX3-0 alone

	// Use Vcc as voltage reference
	ADMUX |= (1 << REFS0);

	//we only need 8-bit precision.  Left adjust the ADC result
	//so that we can read the ADCH register and be done with it.
	ADMUX |= (1 << ADLAR);
}

int main(void)
{
	DDRB |= (1 << OUTPUT_LED) | (1 << OUTPUT_PWM);//led set as output
	DDRC &=~ (1 << POT);//set potentiometer pin for input
	initTimers();
	initAdc();
	// Value for the register
	// "value" equals brightness of the led
	// 0 = off | 255 = full brightness
	uint8_t value = 1;

	while (1)
	{
		value = readAdc();
		_delay_ms(10);
		OCR2 = readAdc();
	}
}
