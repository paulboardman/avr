/*
 * LedCube.c
 *
 *  Created on: Feb 12, 2010
 *      Author: Paul
 */

/*
 * led_cube.c
 *
 *  Created on: September 6th, 2009
 *      Author: Paul
 */
#include <avr/io.h>
#include <avr/delay.h>

//define the cathode pins
//I just happened to connect them to these pins
#define LAYER1 PD1
#define LAYER2 PD0
#define LAYER3 PD2

//define which pin each column (anode) is connected to
uint8_t kColumnConfig[] = { PD3, PB7, PB3, PD4, PD6, PB2, PB6, PD7, PB1 };
//store the layers in an array as well
uint8_t kLayerConfig[] = { LAYER1, LAYER2, LAYER3 };

//this function switches a column on or off
//we have to know which port the column is a part of
//hence the switch statment:
// PORTD pins are at indices 0,3,4,7 in the kColumnConfig array
// PORTB pins are at indices 1,2,5,6,8 in the kColumnConfig array
void setColumn(uint8_t pVal, uint8_t pCol) {
	switch (pCol) {
	//portD pins
	case 0:
	case 3:
	case 4:
	case 7:
		if (pVal) {
			PORTD |= (1 << kColumnConfig[pCol]);
		} else {
			PORTD &= ~(1 << kColumnConfig[pCol]);
		}
		break;
	case 1:
	case 2:
	case 5:
	case 6:
	case 8:
		// portB pins
		if (pVal) {
			PORTB |= (1 << kColumnConfig[pCol]);
		} else {
			PORTB &= ~(1 << kColumnConfig[pCol]);
		}
		break;
	}
}

int main(void) {
	uint8_t i = 0;
	uint8_t j = 0;

	DDRB = 0xFF; // all set to output
	DDRD = 0xFF; // all set to output
	//swich off all layers by applying voltage to the cathode pins thereby preventing current flow
	PORTD |= (1 << LAYER1) | (1 << LAYER2) | (1 << LAYER3);
	while (1) {
		//loop through each layer
		for (i = 0; i < 3; i++) {
			//set the output value for this cathode layer to '0'
			//this allows current to flow in this layer
			PORTD &= ~(1 << kLayerConfig[i]);
			for (j = 0; j < 9; j++) {
				//turn on the LED in column j
				setColumn(1, j);
				_delay_ms(250);
				//trun off the LED in column j
				setColumn(0, j);
			}
			//set the output value for this cathode layer to '1'
			//this prevents current flow for this layer
			PORTD |= (1 << kLayerConfig[i]);
		}
	}
	return 1;
}
