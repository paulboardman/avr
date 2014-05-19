#define F_CPU 1000000UL  // 1 MHz
#include <avr/io.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

//Hardware definitions
#define REDLED      PB2
#define GREENLED    PB3
#define BLUELED     PB4

//Maximim value for led brightness
#define R_MAX 255
#define G_MAX 255
#define B_MAX 255

//Fading States
#define REDtoYELLOW     0
#define YELLOWtoGREEN   1
#define GREENtoCYAN     2
#define CYANtoBLUE      3
#define BLUEtoVIOLET    4
#define VIOLETtoRED     5

unsigned char red;
unsigned char green;
unsigned char blue;
unsigned char state;

void rainbowfade(int n){
	//Go one step through a state machine that fades through the rainbow
	//n sets the step increment
	//255%n must equal 0
	if (state == REDtoYELLOW){
		green += n;
		if(green == G_MAX)
			state++;
	}
	else if (state == YELLOWtoGREEN){
		red -= n;
		if(red == 0)
			state++;
	}
	if (state == GREENtoCYAN){
		blue += n;
		if(blue == B_MAX)
			state++;
	}
	if (state == CYANtoBLUE){
		green -= n;
		if(green == 0)
			state++;
	}
	if (state == BLUEtoVIOLET){
		red += n;
		if(red == R_MAX)
			state++;
	}
	if (state == VIOLETtoRED){
		blue -= n;
		if(blue==0)
			state++;
	}
	//if  (red == V_MAX || green == V_MAX || blue == V_MAX || red == 0 || green == 0 || blue == 0){
		//Finished fading a color; move on to the next
	//	state++;
		state%=6;
	//}
}
int main(void){
	unsigned char i=0;

	//Set pins to output
	DDRB |= _BV(DDB2);
	DDRB |= _BV(DDB1);
	DDRB |= _BV(DDB0);
	DDRB |= _BV(DDB4);
	DDRB |= _BV(DDB3);
	red = 255;
	blue = 0;
	green = 0;
//	DDRB &= ~_BV(DDB3); //Set button to input

//	unsigned char debounce=0;
//	setmode(1); //Start with mode 1
//	timer(); //Let mode set initial color
	unsigned char local_red;
	unsigned char local_green;
	unsigned char local_blue;
	while (1) {
		//Software PWM
		if (i > red){
			PORTB &=~ _BV(REDLED);
		}else{
 			PORTB |= _BV(REDLED);
		}
		if (i > green){
			PORTB &=~ _BV(GREENLED);
		}else{
 			PORTB |= _BV(GREENLED);
		}
		if (i > blue){
			PORTB &=~ _BV(BLUELED);
		}else{
 			PORTB |= _BV(BLUELED);
		}

		if (i==0){ //After blinking LEDs 255 times
			rainbowfade(1);
			local_red = red;
			local_green = green;// >> 2;
			local_blue = blue;// >> 2;
		}
		i++;
	}
	return 0; //Will never get here
}
