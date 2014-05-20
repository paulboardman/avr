/*
* trafflic_lights.c
*
*  Created on: Aug 29, 2009
*      Author: Paul
*/
#include <avr/io.h>
#include <avr/delay.h>

#define RED_LED PB0
#define YELLOW_LED PB1
#define GREEN_LED PB2

#define RED_DELAY 10
#define YELLOW_DELAY 3
#define GREEN_DELAY 15

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

int main(void)
{
  DDRB = 0xFF;//set all to output

  //start traffic light sequence
  while(1)
  {
      PORTB = (1 << RED_LED);
      delay_seconds(RED_DELAY);
      PORTB = (1 << GREEN_LED);
      delay_seconds(GREEN_DELAY);
      PORTB = (1 << YELLOW_LED);
      delay_seconds(YELLOW_DELAY);
  }
}
