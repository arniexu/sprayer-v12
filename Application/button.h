#ifndef BUTTON_H
#define BUTTON_H

#include  "MS51_16K.h"
#include  "nv.h"

// TM1650 button detection  
#define LEFT_UP_BUTTON 0x44
#define LEFT_DOWN_BUTTON 0x45
#define LEFT_SET_BUTTON 0x46
#define RIGHT_UP_BUTTON 0x54
#define RIGHT_DOWN_BUTTON 0x55
#define RIGHT_SET_BUTTON 0x56
#define LEARN_BUTTON 0x64
#define LEFT_SET_LONG_BUTTON 0xa4

// from onboard physical button
#define LEFT_UP_BUTTON_ID 0
#define LEFT_DOWN_BUTTON_ID 1
#define LEFT_SET_BUTTON_ID 2
#define RIGHT_UP_BUTTON_ID 3
#define RIGHT_DOWN_BUTTON_ID 4
#define RIGHT_SET_BUTTON_ID 5
#define LEARN_BUTTON_ID 6

#define NUM_BUTTONS 7

// pin mapping 		pin index			gpio		devboard
// right up				18						p02			B2
// right down			14						p11			B6
// right set			17						p01			B3
// left up				1							p05			A0
// left down			10						p15			A9
// left set				2							p06			A1
// learn					11						p14			B9

// gpio button detections
sbit right_up_button = P0^1;
sbit right_set_button = P1^1;
sbit right_down_button = P0^2;
sbit left_up_button = P0^5;
sbit left_set_button = P1^5;
sbit left_down_button = P0^6;
sbit learn_button = P1^4;

#define RIGHT_UP_BUTTON_PIN_INIT { P02_QUASI_MODE; }
#define	RIGHT_DOWN_BUTTON_PIN_INIT { P11_QUASI_MODE; }
#define RIGHT_SET_BUTTON_PIN_INIT { P01_QUASI_MODE; }
#define LEFT_UP_BUTTON_PIN_INIT { P05_QUASI_MODE; }
#define LEFT_DOWN_BUTTON_PIN_INIT { P15_QUASI_MODE; }
#define LEFT_SET_BUTTON_PIN_INIT { P06_QUASI_MODE; }
#define LEARN_BUTTON_PIN_INIT { P14_QUASI_MODE; }

struct buttonState
{
	unsigned char cod;
	unsigned int dur;
	unsigned int start;
	unsigned int end;
};

void gpio_button_init_interrupt(void);
void gpio_button_init_poll (void);
unsigned char gpio_button_poll_blocked(unsigned char prev);
unsigned char gpio_button_poll_unblocked(void);
unsigned char gpio_button_id(unsigned char cod);
void left_button_logic(unsigned int but);
void right_button_logic(unsigned int but);

#endif