#ifndef OUTPUT_H
#define OUTPUT_H

#include "MS51_16K.h"


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

void input_signal_init();
unsigned char external_button_poll_blocked(void);

void beeper_signal_effective();
void beeper_signal_ineffective();
void relay_effective();
void relay_ineffective();

unsigned char external_start_valid_blocked();
unsigned char external_stop_valid_blocked();
unsigned char external_collaborate_valid_blocked();

#endif