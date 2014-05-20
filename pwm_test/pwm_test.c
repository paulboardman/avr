#import <avr/io.h>
#import <util/delay.h>
#define LED PB0 //output on PB0 which is OC0A
int i = 0;
int main(void)
{
	//set up LED pin for output
	DDRB |= (1 << LED);

	/*
	 * the behaviour of COM0A1 and COM0A0 change depending on the waveform
	 * generation mode set by WGM01:0.
	 */
	TCCR0A |= (1 << COM0A1) | (1 << COM0A0)  // COM0A1 - COM0A0 (Set OC0A on Compare Match, clear OC0A at TOP)
	       | (1 << WGM01)    | (1 << WGM00); // WGM01 - WGM00 (set fast PWM)
	OCR0A   = 0;                              // initialize Output Compare Register A to 0
	TCCR0B |= (1 << CS01);                    // Start timer at Fcpu / 8

	/*
	 * The value in OCR0A (Output Compare Register '0A') is compared with the value in TCNT0 (the timer/counter).
	 * If it matches then OC0A (Output compare '0A') is Set.  when the timer/counter reaches TOP
	 * then OC0A is cleared.
	 */
	//startup handshake (5x 100%, 50%, 0% duty cycles)
	for(i=0;i<5;i++)
	{
		OCR0A = 0;//at this level the LED is fully on
		_delay_ms(250);
		OCR0A=127;//at this level the LED is on 50% of the time
		_delay_ms(250);
		OCR0A=255;//note at this level the LED is off
		_delay_ms(250);
	}

	while(1)
	{
		//de-crease intensity of LED (reduce duty cycle)
		for(i=1;i<255;i++)
		{
			OCR0A = i;
			_delay_ms(10);
		}

		//increase intensity of LED (increase duty cycle)
		for(i=255;i>1;i--)
		{
			OCR0A = i;
			_delay_ms(10);
		}

	}
}
