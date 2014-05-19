/*
 * firefly_jar.c
 *
 * Distributed under Creative Commons 3.0 -- Attib & Share Alike
 *
 *  Created on: Feb 2, 2010
 *      Author: PaulBo
 */
#import <util/delay.h>
#import <avr/io.h>
#import <avr/interrupt.h>

#ifndef F_CPU
    #define F_CPU 8000000UL
#endif

//LED pin definitions
#define LED1    PB0
#define LED2    PB1
#define LED3    PB2
#define LED4    PB3
#define LED5    PB4
#define N_LEDS  5
#define ALL_LEDS (1 << LED1) | (1 << LED2) | (1 << LED3) | (1 << LED4) | (1 << LED5)

//Max value for LED brightness
#define MAX     100

#define PULSE_UP   1
#define PULSE_DOWN 0

volatile unsigned char buffer[N_LEDS];

//counter for use in updateLedState() and pulseLeds()
unsigned char i;

//structure for storing data on each LED
struct ledData {
    unsigned char mBrightness;
    unsigned int  mTime;
    unsigned char mPin;
    unsigned char mPulseDirection;
};

//set up some initial values
struct ledData led_data[] = {
        {0, 1000,  LED1, PULSE_DOWN},
        {0, 10, LED2, PULSE_DOWN},
        {0, 500, LED3, PULSE_DOWN},
        {0, 50, LED4, PULSE_DOWN},
        {0, 150,  LED5, PULSE_DOWN}
};

//return a random time interval from 0 to 255
int getTime()
{
    return TCNT0;//just read the current timer/counter value
}

void updateLedState()
{
    for(i = 0; i < N_LEDS; i++)
    {
        switch(led_data[i].mBrightness)
        {
            case MAX://led is on
                if(--led_data[i].mTime == 0)
                {
                    //decrement the brightness, this puts the LED in a
                    //pulse state
                    led_data[i].mBrightness--;

                    //specify the "down" direction for pulsing
                    led_data[i].mPulseDirection = PULSE_DOWN;
                }
                break;
            case 0://led is off
                if(--led_data[i].mTime == 0)
                {
                    //increment the brightness,this puts the LED in
                    //a pulse state
                    led_data[i].mBrightness++;

                    //specify the "up" direction for pulsing
                    led_data[i].mPulseDirection = PULSE_UP;

                    //set the ON time
                    led_data[i].mTime = getTime() + 1;
                }
                break;
            default: //pulse state
                if(led_data[i].mPulseDirection == PULSE_UP)
                {
                    led_data[i].mBrightness++;
                }
                else
                {
                    if(--led_data[i].mBrightness == 0)
                    {
                        //set the OFF time - make this longer than the on time
                        led_data[i].mTime = (getTime() + 1) * 5;
                    }
                }
                break;
        }
    }
}

void init_timers()
{
    TIMSK0 = (1 << TOIE0);         // enable overflow interrupt
    TCCR0B = (1 << CS00);          // start timer, no prescale

    //enable interrupts
    sei();
}

void init_io()
{
    //set all LED pins as outputs
    DDRB |= ALL_LEDS;
    PORTB &= ~(ALL_LEDS); //off to start
}

void setup()
 {
    init_io();
    init_timers();
}

int main(void)
{
    setup();

    //infinite loop
    while(1)
    {
        updateLedState();
        _delay_ms(10);
    }
}

/*
 * Timer/Counter overflow interrupt. This is called each time
 * the counter overflows (255 counts/cycles).
 */
ISR(TIM0_OVF_vect)
{
    //static variables maintain state from one call to the next
    static unsigned char sPortBmask = ALL_LEDS;
    static unsigned char sCounter = 255;

    //set port pins straight away (no waiting for processing)
    PORTB = sPortBmask;

    //this counter will overflow back to 0 after reaching 255.
    //So we end up adjusting the LED states for every 256 overflows.
    if(++sCounter == 0)
    {
        for(i = 0; i < N_LEDS; i++)
        {
            buffer[i] = led_data[i].mBrightness;
        }
        //set all pins to high
        sPortBmask = ALL_LEDS;
    }
    //this loop is considered for every overflow interrupt.
    //this is the software PWM.
    if(buffer[0] == sCounter) sPortBmask &= ~(1 << led_data[0].mPin);
    if(buffer[1] == sCounter) sPortBmask &= ~(1 << led_data[1].mPin);
    if(buffer[2] == sCounter) sPortBmask &= ~(1 << led_data[2].mPin);
    if(buffer[3] == sCounter) sPortBmask &= ~(1 << led_data[3].mPin);
    if(buffer[4] == sCounter) sPortBmask &= ~(1 << led_data[4].mPin);
}
