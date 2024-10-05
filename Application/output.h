#ifndef OUTPUT_H
#define OUTPUT_H

#include "MS51_16K.h"

#define BEEP_OFF 0
#define BEEP_1HZ 1
#define BEEP_2HZ 2
#define BEEP_4HZ 3
#define BEEP_8HZ 4
#define BEEP_ONCE 0x80
unsigned char beeperFlag = BEEP_OFF;
unsigned int beeper_start = 0;

/* header pin   			mcu pin  			mcu 
// start 							13						p1.2
// end								12						p1.3
// collaborate				6							p1.7
// water_is_short 		5							p3.0
// beeper							19						p0.3
*/

sbit water_short_ex_button = P3^0;
sbit collaborate_ex_button = P1^7;
sbit start_ex_button = P1^2;
sbit stop_ex_button = P1^3;
sbit beeper =	P0^3;
sbit relay = P0^7;

void input_signal_init(void);
unsigned char external_button_poll_blocked(void);

void beeper_signal_effective(void);
void beeper_signal_ineffective(void);
void relay_effective(void);
void relay_ineffective(void);

void beeper_once(void);
void beeper_2hz(void);
void beeper_2hz_stop(void);


unsigned char external_start_valid_blocked(void);
unsigned char external_stop_valid_blocked(void);
unsigned char external_collaborate_valid_blocked(void);

#endif