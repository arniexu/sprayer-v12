/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2020 nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/***********************************************************************************************************/
/* Website: http://www.nuvoton.com                                                                         */
/*  E-Mail : MicroC-8bit@nuvoton.com                                                                       */
/*  Date   : June/21/2020                                                                                   */
/***********************************************************************************************************/

/************************************************************************************************************/
/*  File Function: MS51 DEMO project                                                                        */
/************************************************************************************************************/
#include <RTX51TNY.h>
#include "Application/button.h"
#include "Application/systick.h"
#include "MS51_16K.h"
#include "output.h"
#include "i2c_gpio.h"
#include "timer.h"
#include "I2C_M.h"
#include "tm1650.h"
#include "button.h"
#include "systick.h"
#include "nv.h"
#include <stdio.h>
#include <stdlib.h>

/*
timer 0 is used for rtx51 system tick
timer 2 is used for input capture
timer 1 is used for user system tick
timer 3 is used for printf
uart 0 is used for printf
*/

#define STOP_REASON_USER_ABORT 1
#define STOP_REASON_WATER_SHORT 2

struct TM1650TypeDef tm1650;
struct buttonState prevButton;
unsigned char button;
unsigned char displayBuffer[4] ={0};
#define SOLID_ON 0
#define SLOW_BLINK 1
#define FAST_BLINK 2
#define CIRCULAR_WAITING 3
#define WAIT_WATER 4
unsigned char blinkFlag[4] = {FALSE, FALSE, FALSE, FALSE};
unsigned char dotBlinkFlag[4] = {FALSE, FALSE, FALSE, FALSE};
unsigned char dotFlag[4] = {FALSE, FALSE, FALSE, FALSE};

#define BEEP_OFF 0
#define BEEP_1HZ 1
#define BEEP_2HZ 2
#define BEEP_4HZ 3
#define BEEP_8HZ 4
#define BEEP_ONCE 0x80
unsigned char beeperFlag = BEEP_OFF;
unsigned int beeper_start = 0;
//@TODO: save below 4 variables into flash
unsigned int start_multiplier = 0;
unsigned int start_counter = 0;
unsigned int stop_multiplier = 0;
unsigned int stop_counter = 0;
extern unsigned int tButton;
unsigned int stopReason = 0;
unsigned int level_water_short = 0;
unsigned int delay_water_short = 0;
unsigned int flag_collaborate = 0;

#define LEFT_FUNCTION_1_MODE 1
#define LEFT_FUNCTION_2_MODE 2
#define LEFT_FUNCTION_3_MODE 3
#define LEFT_FUNCTION_4_MODE 4
#define LEFT_FUNCTION_5_MODE 5
#define LEFT_FUNCTION_6_MODE 6
#define LEFT_FUNCTION_7_MODE 7
#define LEFT_FUNCTION_8_MODE 8
#define LEFT_TIME_EFFECTIVE_MODE 9

#define RIGHT_FUNCTION_1_MODE 11
#define RIGHT_FUNCTION_2_MODE 12
#define RIGHT_FUNCTION_3_MODE 13
#define RIGHT_FUNCTION_4_MODE 14
#define RIGHT_FUNCTION_5_MODE 15
#define RIGHT_FUNCTION_6_MODE 16
#define RIGHT_FUNCTION_7_MODE 17
#define RIGHT_FUNCTION_8_MODE 18
#define RIGHT_TIME_EFFECTIVE_MODE 19
#define LEARN_MODE 21

unsigned int learning = TRUE;

unsigned char right_mode = RIGHT_TIME_EFFECTIVE_MODE;
unsigned char left_mode = LEFT_TIME_EFFECTIVE_MODE;

#define SPRAY_STARTING 1
#define SPRAY_STOPPING 2
#define SPRAY_WAITING 3
#define SPRAY_DELAYING 4
#define SPRAY_IDLE 0

unsigned int spraying = SPRAY_IDLE;

unsigned int g_button = 0;

unsigned char start_spray();
unsigned char stop_spray();
void beeper_once();
unsigned int readStopMultiplier();
unsigned int readStartMultiplier();
unsigned char readStopCounter(void);
unsigned char readStartCounter(void);
unsigned char readLeftMode(void);
unsigned char readRightMode(void);
unsigned char readWaterShortLevel(void);
unsigned char readWaterShortDelay(void);
void writeFlash();

void beeper_2hz(void);
void beeper_2hz_stop(void);


unsigned char external_water_short_blocked()
{
	if (water_short_ex_button == level_water_short)
	{
			Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 1);
			if (water_short_ex_button == level_water_short)
				return level_water_short;
	}
	return !level_water_short;
}

void display_when_learning()
{
	if(right_mode == LEARN_MODE && left_mode == LEARN_MODE)
	{
		displayBuffer[0] = '-';
		displayBuffer[1] = '-';
		displayBuffer[2] = '-';
		displayBuffer[3] = '-';
		blinkFlag[0] = blinkFlag[2] = blinkFlag[3] = SOLID_ON;
		blinkFlag[1] = SLOW_BLINK;
		dotFlag[0] = dotFlag[2] = dotFlag[1] = FALSE;
		dotFlag[3] = TRUE;
	}
	//os_set_ready(1);
	//os_switch_task();
}

void display_when_water_is_back()
{
	displayBuffer[0] = 'F';
	displayBuffer[1] = 'C';
	displayBuffer[2] = '0'+ delay_water_short/10;
	displayBuffer[3] = '0'+ delay_water_short%10;
	blinkFlag[0] = blinkFlag[1] = blinkFlag[2] = blinkFlag[3] = SOLID_ON;
	dotFlag[0] = dotFlag[1] = dotFlag[2] = dotFlag[3] = FALSE;
}


void display_when_water_is_short()
{
	// circular waiting
	displayBuffer[0] = '0';
	displayBuffer[1] = '0';
	displayBuffer[2] = '0';
	displayBuffer[3] = '0';
	blinkFlag[0] = blinkFlag[1] = blinkFlag[2] = blinkFlag[3] = WAIT_WATER;
	dotFlag[0] = dotFlag[2] = FALSE;
	dotFlag[1] = dotFlag[3] = TRUE;
}
void display_when_learn_complete()
{
	if (right_mode == LEARN_MODE && left_mode == LEARN_MODE) {
		displayBuffer[0] = '-';
		displayBuffer[1] = '-';
		displayBuffer[2] = '-';
		displayBuffer[3] = '-';
		blinkFlag[0] = blinkFlag[1] = blinkFlag[2] = blinkFlag[3] = FAST_BLINK;
		dotFlag[0] = dotFlag[1] = dotFlag[2] = FALSE;
		dotFlag[3] = FALSE;
	}
	//os_set_ready(1);
	//os_switch_task();
}

// left business logic
void left_business_logic() 
{
		switch(left_mode)
		{
			case LEFT_FUNCTION_1_MODE:
				displayBuffer[0] = 'F';
				displayBuffer[1] = '1';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				dotFlag[0] = dotFlag[1] = FALSE;
				start_multiplier = 40; // 0.1s
				// wait_button_blocked
				if (button == LEFT_SET_BUTTON)
					left_mode = LEFT_TIME_EFFECTIVE_MODE;
				else if(button == LEFT_UP_BUTTON)
					left_mode = LEFT_FUNCTION_2_MODE;
				else if(button == LEFT_DOWN_BUTTON)
					left_mode = LEFT_FUNCTION_6_MODE;
				else if (button == LEARN_BUTTON)
				{
					left_mode = LEARN_MODE;
				}
				else if (button == LEFT_SET_LONG_BUTTON)
				{
					left_mode = LEFT_FUNCTION_7_MODE;
					right_mode = RIGHT_FUNCTION_7_MODE;
				}
				break;
			case LEFT_FUNCTION_2_MODE:
				displayBuffer[0] = 'F';
				displayBuffer[1] = '2';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				dotFlag[0] = dotFlag[1] = FALSE;
				start_multiplier = 400; // 1s
				// wait_button_blocked
				if (button == LEFT_SET_BUTTON)
					left_mode = LEFT_TIME_EFFECTIVE_MODE;
				else if(button == LEFT_UP_BUTTON)
					left_mode = LEFT_FUNCTION_3_MODE;
				else if(button == LEFT_DOWN_BUTTON)
					left_mode = LEFT_FUNCTION_1_MODE;
				else if (button == LEARN_BUTTON)
				{
					left_mode = LEARN_MODE;
				}
				else if (button == LEFT_SET_LONG_BUTTON)
				{
					left_mode = LEFT_FUNCTION_7_MODE;
					right_mode = RIGHT_FUNCTION_7_MODE;
				}
				break;
			case LEFT_FUNCTION_3_MODE:
				displayBuffer[0] = 'F';
				displayBuffer[1] = '3';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				dotFlag[0] = dotFlag[1] = FALSE;
				start_multiplier = 2400; //0.1min
				// wait_button_blocked
				if (button == LEFT_SET_BUTTON)
					left_mode = LEFT_TIME_EFFECTIVE_MODE;
				else if(button == LEFT_UP_BUTTON)
					left_mode = LEFT_FUNCTION_4_MODE;
				else if(button == LEFT_DOWN_BUTTON)
					left_mode = LEFT_FUNCTION_2_MODE;
				else if (button == LEARN_BUTTON)
				{
					left_mode = LEARN_MODE;
				}
				else if (button == LEFT_SET_LONG_BUTTON)
				{
					left_mode = LEFT_FUNCTION_7_MODE;
					right_mode = RIGHT_FUNCTION_7_MODE;
				}
				break;
			case LEFT_FUNCTION_4_MODE:
				displayBuffer[0] = 'F';
				displayBuffer[1] = '4';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				dotFlag[0] = dotFlag[1] = FALSE;
				start_multiplier = 24000; //1min
				// wait_button_blocked
				if (button == LEFT_SET_BUTTON)
					left_mode = LEFT_TIME_EFFECTIVE_MODE;
				else if(button == LEFT_UP_BUTTON)
					left_mode = LEFT_FUNCTION_5_MODE;
				else if(button == LEFT_DOWN_BUTTON)
					left_mode = LEFT_FUNCTION_3_MODE;
				else if (button == LEARN_BUTTON)
				{
					left_mode = LEARN_MODE;
				}
				else if (button == LEFT_SET_LONG_BUTTON)
				{
					left_mode = LEFT_FUNCTION_7_MODE;
					right_mode = RIGHT_FUNCTION_7_MODE;
				}
				break;
			case LEFT_FUNCTION_5_MODE:
				displayBuffer[0] = 'F';
				displayBuffer[1] = '5';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				dotFlag[0] = dotFlag[1] = FALSE;
				start_multiplier = 144000; //6 min
				// wait_button_blocked
				if (button == LEFT_SET_BUTTON)
					left_mode = LEFT_TIME_EFFECTIVE_MODE;
				else if(button == LEFT_UP_BUTTON)
					left_mode = LEFT_FUNCTION_6_MODE;
				else if(button == LEFT_DOWN_BUTTON)
					left_mode = LEFT_FUNCTION_4_MODE;
				else if (button == LEARN_BUTTON)
				{
					left_mode = LEARN_MODE;
				}
				else if (button == LEFT_SET_LONG_BUTTON)
				{
					left_mode = LEFT_FUNCTION_7_MODE;
					right_mode = RIGHT_FUNCTION_7_MODE;
				}
				break;
			case LEFT_FUNCTION_6_MODE:
				displayBuffer[0] = 'F';
				displayBuffer[1] = '6';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				dotFlag[0] = dotFlag[1] = FALSE;
				start_multiplier = 60*60*400; //60 min
				// wait_button_blocked
				if (button == LEFT_SET_BUTTON)
					left_mode = LEFT_TIME_EFFECTIVE_MODE;
				else if(button == LEFT_UP_BUTTON)
					left_mode = LEFT_FUNCTION_1_MODE;
				else if(button == LEFT_DOWN_BUTTON)
					left_mode = LEFT_FUNCTION_5_MODE;
				else if (button == LEARN_BUTTON)
				{
					left_mode = LEARN_MODE;
				}
				else if (button == LEFT_SET_LONG_BUTTON)
				{
					left_mode = LEFT_FUNCTION_7_MODE;
					right_mode = RIGHT_FUNCTION_7_MODE;
				}
				break;
			case LEFT_TIME_EFFECTIVE_MODE:
				if(spraying == SPRAY_IDLE || spraying == SPRAY_STARTING)
				{
					blinkFlag[0] = blinkFlag[1] = FALSE;
					displayBuffer[0] = '0'+(start_counter/10)%10;
					displayBuffer[1] = '0'+start_counter%10;
					if(start_multiplier == 40
						|| start_multiplier == 40*10*6
						|| start_multiplier == 40*10*6*10*6)
					{
						dotFlag[0] = TRUE;
						dotFlag[1] = FALSE;
					}
					else
						dotFlag[0] = dotFlag[1] = FALSE;
				}
				else if(spraying == SPRAY_WAITING)
				{
					display_when_water_is_short();
				}
				else if(spraying == SPRAY_DELAYING)
				{
					display_when_water_is_back();
				}
				else
				{
					blinkFlag[0] = blinkFlag[1] = FALSE;
					displayBuffer[0] = 0;
					displayBuffer[1] = 0;
					dotFlag[0] = dotFlag[1] = FALSE;
				}
				// wait_button_blocked
				if (button == LEFT_SET_BUTTON)
					left_mode = readLeftMode();
				else if(button == LEFT_UP_BUTTON)
				{
					if (start_counter == 99)
						start_counter = 1;
					else
						start_counter ++;

				}
				else if(button == LEFT_DOWN_BUTTON)
				{
					if (start_counter == 1)
						start_counter = 99;
					else if (start_counter > 1)
						start_counter --;
				}
				else if (button == LEARN_BUTTON)
				{
					left_mode = LEARN_MODE;
				}
				else if (button == LEFT_SET_LONG_BUTTON)
				{
					left_mode = LEFT_FUNCTION_7_MODE;
					right_mode = RIGHT_FUNCTION_7_MODE;
				}
				break;
			case LEARN_MODE:
				if (learning)
				{
					display_when_learning();
				}
				else
				{
					display_when_learn_complete();
				}
				break;
			case LEFT_FUNCTION_7_MODE:
				dotFlag[0] = dotFlag[1] = FALSE;
				displayBuffer[0] = 'F';
				displayBuffer[1] = '7';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				if(button == LEFT_SET_BUTTON){
					right_mode = RIGHT_FUNCTION_8_MODE;
					left_mode = LEFT_FUNCTION_8_MODE;
				}
				break;
			case LEFT_FUNCTION_8_MODE:
				dotFlag[0] = dotFlag[1] = FALSE;
				displayBuffer[0] = 'F';
				displayBuffer[1] = '8';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				if (button == LEFT_SET_BUTTON)
				{
					left_mode = LEFT_TIME_EFFECTIVE_MODE;
					right_mode = RIGHT_TIME_EFFECTIVE_MODE;
				}
				break;
			default:
				break;
		}
	return;
}

unsigned int water_is_not_short()
{
	unsigned int water_present = TRUE;
	unsigned int now = get_Timer1_Systemtick();
	unsigned int delay_water_short_backup = 0;
	level_water_short = readWaterShortLevel();
	delay_water_short = readWaterShortDelay();
	delay_water_short_backup = delay_water_short;
	while (level_water_short == external_water_short_blocked() && get_Timer1_Systemtick() - now < 30*60*400)
	{
		relay_ineffective();
		spraying = SPRAY_WAITING;
		display_when_water_is_short();
		water_present = FALSE;
		beeper_2hz();
		if (tButton == RF_STOP_BUTTON
		|| RF_STOP_BUTTON == external_button_poll_blocked()
		|| (flag_collaborate && collaborate_ex_button == 1))
		{

			beeper_once();
			tButton = 0;
			return FALSE;
		}
		else if(tButton == RF_START_BUTTON)
		{
			beeper_once();
			tButton = 0;
		}
	}
	beeper_2hz_stop();
	if(water_present == FALSE  && level_water_short == external_water_short_blocked()) // water is still short 
	{
		relay_ineffective();
		spraying = SPRAY_IDLE;
		return FALSE;
	}
	else if(water_present == FALSE && level_water_short != external_water_short_blocked()) // water is not short now
	{
		spraying = SPRAY_DELAYING;
		while(0 < delay_water_short)
		{
			unsigned int temp = 0;
			display_when_water_is_back();
			temp = get_Timer1_Systemtick();
			while (get_Timer1_Systemtick() - temp < 400) {
				if(level_water_short == external_water_short_blocked())
				{
					return TRUE;
				}
				else if (tButton == RF_STOP_BUTTON
				|| RF_STOP_BUTTON == external_button_poll_blocked())
				{
					return FALSE;
				}
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 1);
			}
			delay_water_short --;
		}
		delay_water_short = delay_water_short_backup;
		Write_DATAFLASH_BYTE(DATA_START_ADDR+DELAY_WATER_SHORT_OFFSET, delay_water_short%100);
		return TRUE;
	}else if (water_present) // water is never short
	{
		return TRUE;
	}
	else{
		spraying = SPRAY_IDLE;
		return FALSE;
	}
}
unsigned char start_spray(void)
{
	unsigned int start_counter_backup = start_counter%100;
	unsigned int cnt = start_multiplier;
	unsigned int now = 0;
	if (left_mode == LEFT_TIME_EFFECTIVE_MODE)
	{
		while(start_counter>0)
		{
			// start spraying
			relay_effective();
			spraying = SPRAY_STARTING;
			cnt = start_multiplier;
			now = get_Timer1_Systemtick();
			while(get_Timer1_Systemtick() - now < cnt){
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				if (tButton == RF_STOP_BUTTON
					|| RF_STOP_BUTTON == external_button_poll_blocked()
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					beeper_once();
					relay_ineffective();
					stopReason = STOP_REASON_USER_ABORT;
					start_counter = start_counter_backup;
					Write_DATAFLASH_BYTE(DATA_START_ADDR+START_COUNTER_OFFSET, start_counter%100);
					return FALSE;
				}
				else if(tButton == RF_START_BUTTON)
				{
					beeper_once();
					tButton = 0;
				}
				else if(level_water_short == external_water_short_blocked())
				{
					relay_ineffective();
					stopReason = STOP_REASON_WATER_SHORT;
					start_counter = start_counter_backup;
					Write_DATAFLASH_BYTE(DATA_START_ADDR+START_COUNTER_OFFSET, start_counter%100);
					return TRUE;
				}
			}
			start_counter --;
		}
	}
	start_counter = start_counter_backup;
	Write_DATAFLASH_BYTE(DATA_START_ADDR+START_COUNTER_OFFSET, start_counter);
	return TRUE;
}

unsigned char stop_spray(void)
{
	unsigned int cnt = stop_multiplier;
	unsigned int stop_counter_backup = stop_counter%100;
	unsigned int now = 0;
	if (right_mode == RIGHT_TIME_EFFECTIVE_MODE)
	{
		while(stop_counter > 0)
		{
			relay_ineffective();
			spraying = SPRAY_STOPPING;
			cnt = stop_multiplier;
			now =  get_Timer1_Systemtick();
			while(get_Timer1_Systemtick() - now < cnt){
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				if (tButton == RF_STOP_BUTTON
					|| RF_STOP_BUTTON == external_button_poll_blocked()
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					beeper_once();
					stopReason = STOP_REASON_USER_ABORT;
					stop_counter = stop_counter_backup;
					Write_DATAFLASH_BYTE(DATA_START_ADDR+STOP_COUNTER_OFFSET, stop_counter%100);
					return FALSE;
				}
				else if(tButton == RF_START_BUTTON)
				{
					beeper_once();
					tButton = 0;
				}
				else if (level_water_short == external_water_short_blocked())
				{
					stopReason = STOP_REASON_WATER_SHORT;
					stop_counter = stop_counter_backup;
					Write_DATAFLASH_BYTE(DATA_START_ADDR+STOP_COUNTER_OFFSET, stop_counter%100);
					return TRUE;
				}
			}
			stop_counter --;
		}
	}
	stop_counter = stop_counter_backup;
	Write_DATAFLASH_BYTE(DATA_START_ADDR+STOP_COUNTER_OFFSET, stop_counter%100);
	return TRUE;
}

void learn_code(void)
{
	unsigned int now = get_Timer1_Systemtick();
	unsigned int now1 = 0;
	while(get_Timer1_Systemtick() - now < 400*8 && tButton == 0)
	{
		learning = TRUE;
		display_when_learning();
		Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
	}
	if(tButton == RF_START_BUTTON || tButton == RF_STOP_BUTTON)
	{
		learning = FALSE;
		saveToFlash();
		beeper_once();
		//writeAddressToFlash();
		display_when_learn_complete();
		now1 = get_Timer1_Systemtick();
		while(get_Timer1_Systemtick() -	now1 < 400)
			Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
		left_mode = LEFT_TIME_EFFECTIVE_MODE;
		right_mode = RIGHT_TIME_EFFECTIVE_MODE;
		tButton = 0;
		return;
	}
	learning = FALSE;
	display_when_learn_complete();
	Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 800);
	left_mode = LEFT_TIME_EFFECTIVE_MODE;
	right_mode = RIGHT_TIME_EFFECTIVE_MODE;
	tButton = 0;
	return;
}

// right business logic
void right_business_logic() 
{
		switch(right_mode)
		{
			case RIGHT_FUNCTION_1_MODE:
				displayBuffer[2] = 'F';
				displayBuffer[3] = '1';
				blinkFlag[2] = FALSE;
				blinkFlag[3] = TRUE;
				dotFlag[2] = dotFlag[3] = FALSE;
				stop_multiplier = 40; // 0.1s
				// wait_button_blocked
				if (button == RIGHT_SET_BUTTON)
					right_mode = RIGHT_TIME_EFFECTIVE_MODE;
				else if(button == RIGHT_UP_BUTTON)
					right_mode = RIGHT_FUNCTION_2_MODE;
				else if(button == RIGHT_DOWN_BUTTON)
					right_mode = RIGHT_FUNCTION_6_MODE;
				else if (button == LEARN_BUTTON)
				{
					right_mode = LEARN_MODE;
				}
				break;
			case RIGHT_FUNCTION_2_MODE:
				displayBuffer[2] = 'F';
				displayBuffer[3] = '2';
				blinkFlag[2] = FALSE;
				blinkFlag[3] = TRUE;
				dotFlag[2] = dotFlag[3] = FALSE;
				stop_multiplier = 400; // 1s
				// wait_button_blocked
				if (button == RIGHT_SET_BUTTON)
					right_mode = RIGHT_TIME_EFFECTIVE_MODE;
				else if(button == RIGHT_UP_BUTTON)
					right_mode = RIGHT_FUNCTION_3_MODE;
				else if(button == RIGHT_DOWN_BUTTON)
					right_mode = RIGHT_FUNCTION_1_MODE;
				else if (button == LEARN_BUTTON)
				{
					right_mode = LEARN_MODE;
				}
				break;
			case RIGHT_FUNCTION_3_MODE:
				displayBuffer[2] = 'F';
				displayBuffer[3] = '3';
				blinkFlag[2] = FALSE;
				blinkFlag[3] = TRUE;
				dotFlag[2] = dotFlag[3] = FALSE;
				stop_multiplier = 2400; //0.1min
				// wait_button_blocked
				if (button == RIGHT_SET_BUTTON)
					right_mode = RIGHT_TIME_EFFECTIVE_MODE;
				else if(button == RIGHT_UP_BUTTON)
					right_mode = RIGHT_FUNCTION_4_MODE;
				else if(button == RIGHT_DOWN_BUTTON)
					right_mode = RIGHT_FUNCTION_2_MODE;
				else if (button == LEARN_BUTTON)
				{
					right_mode = LEARN_MODE;
				}
				break;
			case RIGHT_FUNCTION_4_MODE:
				displayBuffer[2] = 'F';
				displayBuffer[3] = '4';
				blinkFlag[2] = FALSE;
				blinkFlag[3] = TRUE;
				dotFlag[2] = dotFlag[3] = FALSE;
				stop_multiplier = 60*400; //1min
				// wait_button_blocked
				if (button == RIGHT_SET_BUTTON)
					right_mode = RIGHT_TIME_EFFECTIVE_MODE;
				else if(button == RIGHT_UP_BUTTON)
					right_mode = RIGHT_FUNCTION_5_MODE;
				else if(button == RIGHT_DOWN_BUTTON)
					right_mode = RIGHT_FUNCTION_3_MODE;
				else if (button == LEARN_BUTTON)
				{
					right_mode = LEARN_MODE;
				}
				break;
			case RIGHT_FUNCTION_5_MODE:
				displayBuffer[2] = 'F';
				displayBuffer[3] = '5';
				blinkFlag[2] = FALSE;
				blinkFlag[3] = TRUE;
				dotFlag[2] = dotFlag[3] = FALSE;
				stop_multiplier = 6*60*400; //6 min
				// wait_button_blocked
				if (button == RIGHT_SET_BUTTON)
					right_mode = RIGHT_TIME_EFFECTIVE_MODE;
				else if(button == RIGHT_UP_BUTTON)
					right_mode = RIGHT_FUNCTION_6_MODE;
				else if(button == RIGHT_DOWN_BUTTON)
					right_mode = RIGHT_FUNCTION_4_MODE;
				else if (button == LEARN_BUTTON)
				{
					right_mode = LEARN_MODE;
				}
				break;
			case RIGHT_FUNCTION_6_MODE:
				displayBuffer[2] = 'F';
				displayBuffer[3] = '6';
				blinkFlag[2] = FALSE;
				blinkFlag[3] = TRUE;
				dotFlag[2] = dotFlag[3] = FALSE;
				stop_multiplier = 60*60*400; //60 min
				// wait_button_blocked
				if (button == RIGHT_SET_BUTTON)
					right_mode = RIGHT_TIME_EFFECTIVE_MODE;
				else if(button == RIGHT_UP_BUTTON)
					right_mode = RIGHT_FUNCTION_1_MODE;
				else if(button == RIGHT_DOWN_BUTTON)
					right_mode = RIGHT_FUNCTION_5_MODE;
				else if (button == LEARN_BUTTON)
				{
					right_mode = LEARN_MODE;
				}
				break;

			case RIGHT_TIME_EFFECTIVE_MODE:
				if (spraying == SPRAY_IDLE || spraying == SPRAY_STOPPING)
				{
					if(stop_multiplier == 40
						|| stop_multiplier == 40*10*6
						|| stop_multiplier == 40*10*6*10*6)
					{
						dotFlag[2] = TRUE;
						dotFlag[3] = FALSE;
					}
					else
						dotFlag[2] = dotFlag[3] = FALSE;
					displayBuffer[3] = '0'+stop_counter%10;
					displayBuffer[2] = '0'+(stop_counter/10)%10;
				}
				else if(spraying == SPRAY_WAITING)
				{
					display_when_water_is_short();
				}
				else if (spraying == SPRAY_DELAYING)
				{
					display_when_water_is_back();
				}
				else
				{
					dotFlag[2] = dotFlag[3] = FALSE;
					displayBuffer[3] = 0;
					displayBuffer[2] = 0;
				}
				// wait_button_blocked
				if (button == RIGHT_SET_BUTTON)
					right_mode = readRightMode();
				else if(button == RIGHT_UP_BUTTON)
				{
					if (stop_counter == 99)
						stop_counter = 0;
					else
						stop_counter ++;

				}
				else if(button == RIGHT_DOWN_BUTTON)
				{
					if (stop_counter == 0)
						stop_counter = 99;
					else if(stop_counter > 0)
						stop_counter --;
				}
				else if (button == LEARN_BUTTON)
				{
					right_mode = LEARN_MODE;
				}
				break;
			case LEARN_MODE: 
				if(learning)
					display_when_learning();
				else
					display_when_learn_complete();
				break;
			case RIGHT_FUNCTION_7_MODE:
				dotFlag[2] = FALSE;
				dotFlag[3] = TRUE;
				displayBuffer[2] = 0;
				displayBuffer[3] = '0' + level_water_short%2;
				blinkFlag[2] = FALSE;
				blinkFlag[3] = TRUE;
				if(button == RIGHT_UP_BUTTON || button == RIGHT_DOWN_BUTTON)
					level_water_short = (level_water_short == 0 ? 1 : 0);
				break;
			case RIGHT_FUNCTION_8_MODE:
				dotFlag[2] = TRUE;
				dotFlag[3] = TRUE;
				displayBuffer[2] = '0'+ (delay_water_short/10)%10;
				displayBuffer[3] = '0' + delay_water_short%10;
				blinkFlag[2] = blinkFlag[3] = TRUE;
				if(button == RIGHT_UP_BUTTON)
					delay_water_short ++;
				else if(button == RIGHT_DOWN_BUTTON)
					delay_water_short = delay_water_short > 0 ? delay_water_short - 1 : 0;

				break;
			default:
				break;
		}
	return;
}

void business_logic() _task_ 2
{
	unsigned char previous = 0;
	gpio_button_init_poll();
	input_signal_init();
	start_counter = readStartCounter()%100;
	stop_counter = readStopCounter()%100;
	start_multiplier = readStartMultiplier();
	stop_multiplier = readStopMultiplier();
	level_water_short = readWaterShortLevel()%2;
	delay_water_short = readWaterShortDelay()%100;
	left_business_logic();
	right_business_logic();
	while(1)
	{
		button = gpio_button_poll_blocked(previous);
		if (isButtonCodeValid(button) && spraying == SPRAY_IDLE)
		{
			if (previous != button)  // button is not pressed consecutively
				beeper_once();
			left_business_logic();
			right_business_logic();
			previous = button;
			button = 0;
			left_business_logic();
			right_business_logic();
			writeFlash();
			if (right_mode == LEARN_MODE
				&& left_mode == LEARN_MODE)
			{
				learn_code();
			}
		}
		//previous = button;
	}
}

// refresh led display
void refresh_led (void) _task_ 1
{
	unsigned int i = 0;
	// start a timer as system tick handler, time out every 1ms 
	// count the number of	
	start_counter = readStartCounter()%100;
	stop_counter = readStopCounter()%100;
	start_multiplier = readStartMultiplier();
	stop_multiplier = readStopMultiplier();
	level_water_short = readWaterShortLevel();
	delay_water_short = readWaterShortDelay();
	IIC_Init();
	tm1650_displayOn(&tm1650);
	while(1)
	{
		#if 1
		switch(left_mode)
		{
			case LEFT_FUNCTION_1_MODE:
			case LEFT_FUNCTION_2_MODE:
			case LEFT_FUNCTION_3_MODE:
			case LEFT_FUNCTION_4_MODE:
			case LEFT_FUNCTION_5_MODE:
			case LEFT_FUNCTION_6_MODE:
				displayBuffer[0] = 'F';
				if (left_mode == LEFT_FUNCTION_1_MODE)
				{
					displayBuffer[1] = '1';
				}
				else if (left_mode == LEFT_FUNCTION_2_MODE)
				{
					displayBuffer[1] = '2';
				}
				else if (left_mode == LEFT_FUNCTION_3_MODE)
				{
					displayBuffer[1] = '3';
				}
				else if (left_mode == LEFT_FUNCTION_4_MODE)
				{
					displayBuffer[1] = '4';
				}
				else if (left_mode == LEFT_FUNCTION_5_MODE)
				{
					displayBuffer[1] = '5';
				}
				else if (left_mode == LEFT_FUNCTION_6_MODE)
				{
					displayBuffer[1] = '6';
				}
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				break;
			case LEARN_MODE:
				if (learning)
				{
					display_when_learning();
				}
				else
				{
					display_when_learn_complete();
				}
				break;
			case LEFT_TIME_EFFECTIVE_MODE:
				if (spraying == SPRAY_IDLE || spraying == SPRAY_STARTING)
				{
					blinkFlag[0] = blinkFlag[1] = FALSE;
					displayBuffer[1] = '0'+start_counter%10;
					displayBuffer[0] = '0'+(start_counter/10)%10;
					if(start_multiplier == 40
						|| start_multiplier == 40*10*6
						|| start_multiplier == 40*10*6*10*6)
					{
						dotFlag[0] = TRUE;
						dotFlag[1] = FALSE;
					}
					else
						dotFlag[0] = dotFlag[1] = FALSE;
				}
				else if(spraying == SPRAY_WAITING)
				{
					display_when_water_is_short();
				}
				else if(spraying == SPRAY_DELAYING)
				{
					display_when_water_is_back();
				}
				else
				{
					displayBuffer[1] = 0;
					displayBuffer[0] = 0;
					dotFlag[0] = dotFlag[1] = FALSE;
					blinkFlag[0] = blinkFlag[1] = FALSE;
				}
				break;
			case LEFT_FUNCTION_7_MODE:
				dotFlag[0] = dotFlag[1] = FALSE;
				displayBuffer[0] = 'F';
				displayBuffer[1] = '7';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				break;
			case LEFT_FUNCTION_8_MODE:
				dotFlag[0] = dotFlag[1] = FALSE;
				displayBuffer[0] = 'F';
				displayBuffer[1] = '8';
				blinkFlag[0] = FALSE;
				blinkFlag[1] = TRUE;
				break;
			default:
				break;
		}
		switch(right_mode)
		{
			case RIGHT_FUNCTION_1_MODE:
			case RIGHT_FUNCTION_2_MODE:
			case RIGHT_FUNCTION_3_MODE:
			case RIGHT_FUNCTION_4_MODE:
			case RIGHT_FUNCTION_5_MODE:
			case RIGHT_FUNCTION_6_MODE:
				displayBuffer[2] = 'F';
				if (right_mode == RIGHT_FUNCTION_1_MODE)
				{
					displayBuffer[3] = '1';
				}
				else if (right_mode == RIGHT_FUNCTION_2_MODE)
				{
					displayBuffer[3] = '2';
				}
				else if (right_mode == RIGHT_FUNCTION_3_MODE)
				{
					displayBuffer[3] = '3';
				}
				else if (right_mode == RIGHT_FUNCTION_4_MODE)
				{
					displayBuffer[3] = '4';
				}
				else if (right_mode == RIGHT_FUNCTION_5_MODE)
				{
					displayBuffer[3] = '5';
				}
				else if (right_mode == RIGHT_FUNCTION_6_MODE)
				{
					displayBuffer[3] = '6';
				}
				blinkFlag[2] = FALSE;
				blinkFlag[3] = TRUE;
				break;
			case LEARN_MODE:
				if (learning)
				{
					display_when_learning();
				}
				else
				{
					display_when_learn_complete();
				}
				break;
			case RIGHT_TIME_EFFECTIVE_MODE:
				if (spraying == SPRAY_IDLE || spraying == SPRAY_STOPPING)
				{
					displayBuffer[3] = '0'+stop_counter%10;
					displayBuffer[2] = '0'+(stop_counter/10)%10;
					blinkFlag[2] = blinkFlag[3] = FALSE;
					if(stop_multiplier == 40
						|| stop_multiplier == 40*10*6
						|| stop_multiplier == 40*10*6*10*6)
					{
						dotFlag[2] = TRUE;
						dotFlag[3] = FALSE;
					}
					else
						dotFlag[2] = dotFlag[3] = FALSE;
				}
				else if(spraying == SPRAY_WAITING)
				{
					display_when_water_is_short();
				}
				else if(spraying == SPRAY_DELAYING)
				{
					display_when_water_is_back();
				}
				else
				{
					displayBuffer[3] = 0;
					displayBuffer[2] = 0;
					dotFlag[2] = dotFlag[3] = FALSE;
					blinkFlag[2] = blinkFlag[3] = FALSE;
				}
				break;
			case RIGHT_FUNCTION_7_MODE:
				dotFlag[2] = FALSE;
				dotFlag[3] = TRUE;
				blinkFlag[2] = FALSE;
				blinkFlag[3] = TRUE;
				displayBuffer[2] = 0;
				displayBuffer[3] = '0' + level_water_short%2;
				break;
			case RIGHT_FUNCTION_8_MODE:
				dotFlag[2] = FALSE;
				dotFlag[3] = FALSE;
				displayBuffer[2] = '0'+ (delay_water_short/10)%10;
				displayBuffer[3] = '0' + delay_water_short%10;
				blinkFlag[2] = blinkFlag[3] = TRUE;
				break;
			default:
				break;
		}
		#endif

		for (i=0; i<4; i++)
		{
			if(blinkFlag[i])
			{
				unsigned short period = 0;
				if (blinkFlag[i] == FAST_BLINK)
				{
					period = 100;
					if(get_Timer1_Systemtick()%period < period/2)
					{
						tm1650_displayChar(&tm1650, i, 0);
					}
					else
					{
						if(dotFlag[i])
							tm1650_displayChar_withDot(&tm1650, i, displayBuffer[i]);
						else
							tm1650_displayChar(&tm1650, i, displayBuffer[i]);
					}
				}
				else if(blinkFlag[i] == SLOW_BLINK)
				{
					period = 400;
					if(get_Timer1_Systemtick()%period < period/2)
					{
						tm1650_displayChar(&tm1650, i, 0);
					}
					else
					{
						if(dotFlag[i])
							tm1650_displayChar_withDot(&tm1650, i, displayBuffer[i]);
						else
							tm1650_displayChar(&tm1650, i, displayBuffer[i]);
					}
				}
				else if(blinkFlag[i] == CIRCULAR_WAITING)
				{
					period = 1200;
					if(get_Timer1_Systemtick()%1200 < 200 && get_Timer1_Systemtick()%1200 > 0)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],1);
					else if(get_Timer1_Systemtick()%1200 < 400 && get_Timer1_Systemtick()%1200 > 200)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x02);
					else if(get_Timer1_Systemtick()%1200 < 600 && get_Timer1_Systemtick()%1200 > 400)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x03);
					else if(get_Timer1_Systemtick()%1200 < 800 && get_Timer1_Systemtick()%1200 > 600)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x04);
					else if(get_Timer1_Systemtick()%1200 < 1000 && get_Timer1_Systemtick()%1200 > 800)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x5);
					else if(get_Timer1_Systemtick()%1200 < 1200 && get_Timer1_Systemtick()%1200 > 1000)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x06);
				}
				else if(blinkFlag[i] == WAIT_WATER)
				{
					tm1650_displayChar_withDot_underMask(&tm1650, i, displayBuffer[i], 0x8c);
				}
			}
			else
			{
				if(dotFlag[i])
					tm1650_displayChar_withDot(&tm1650, i, displayBuffer[i]);
				else
					tm1650_displayChar(&tm1650, i, displayBuffer[i]);
			}
			if(beeperFlag)
			{
				unsigned int period = 0;
				if(beeperFlag & BEEP_ONCE)
				{
					if(get_Timer1_Systemtick() - beeper_start < 80)
						beeper_signal_effective();
					else
					{
						beeper_start = 0;
						beeperFlag &= ~BEEP_ONCE;
						beeper_signal_ineffective();
					}
					continue;
				}
				if (beeperFlag == BEEP_1HZ)
				{
					period = 400;
				}
				else if(beeperFlag == BEEP_2HZ)
					period = 200;
				else if(beeperFlag == BEEP_4HZ)
					period = 100;
				else if(beeperFlag == BEEP_8HZ)
					period = 50;
				if (get_Timer1_Systemtick()%period < period/2)
				{
					beeper_signal_effective();
				}
				else
				{
					beeper_signal_ineffective();
				}
			}
			else 
			{
				beeper_signal_ineffective();	
			}
		}
		//os_switch_task();
	}
}

// start up task to bring other tasks up
int startup_task (void) _task_ 0
{
	unsigned int start = 0, end = 0;
	MODIFY_HIRC(HIRC_24);
	P10_QUASI_MODE;
	enable_Timer2_IC2();
	start_Timer1_Systemtick();
	input_signal_init();
	beeper = 0;
	os_create_task(1);
	os_create_task(2);
	relay_ineffective();
	//os_delete_task(0);
	while(1){
		start = get_Timer1_Systemtick();
		while(P10 == 0);
		end = get_Timer1_Systemtick();
		if (end - start > 3)
		{
			enable_Timer2_IC2();
		}
		//spraying = SPRAY_IDLE;
		//printf("\n Hello world!");
		//printf("\n Hello world!\r\n");
		if (right_mode == RIGHT_TIME_EFFECTIVE_MODE
			&& left_mode == LEFT_TIME_EFFECTIVE_MODE)
		{
			if (level_water_short != external_water_short_blocked())
			{
				beeper_2hz_stop();
				spraying = SPRAY_IDLE;
				if (tButton == RF_START_BUTTON  // start is pressed from the remote controller
					|| RF_START_BUTTON == external_button_poll_blocked()) 
				{
					beeper_once();
					tButton = 0;
					while(water_is_not_short() && start_spray() && stop_spray())
					{
						Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
						start_counter = readStartCounter();
						stop_counter = readStopCounter();
						stop_multiplier = readStopMultiplier();
						start_multiplier = readStartMultiplier();
					}
					spraying = SPRAY_IDLE;
				}
				else if(collaborate_ex_button == 0)
				{
					beeper_once();
					flag_collaborate = 1;
					while(collaborate_ex_button == 0 && water_is_not_short() && start_spray() && stop_spray())
					{
						Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
						start_counter = readStartCounter();
						stop_counter = readStopCounter();
						stop_multiplier = readStopMultiplier();
						start_multiplier = readStartMultiplier();
					}
					flag_collaborate = 0;
					spraying = SPRAY_IDLE;
				}
			}
			else 
			{
				relay_ineffective();
				display_when_water_is_short();
				spraying = SPRAY_WAITING;
				beeper_2hz();
			}
		}
		if(tButton != 0)
		{
			beeper_once();
			tButton = 0;
		}
		os_switch_task();
	}
}

void beeper_once(void) 
{
	beeper_start = get_Timer1_Systemtick();
	beeperFlag |= BEEP_ONCE;
}

void beeper_2hz(void)
{
	beeperFlag &= ~0x7f;
	beeperFlag += BEEP_2HZ;
}

void beeper_2hz_stop()
{
	beeperFlag &= ~0x7f;
}
unsigned int readStopMultiplier()
{
	unsigned int val = 0;
	Read_DATAFLASH_ARRAY(DATA_START_ADDR+ STOP_MULTIPLIER_OFFSET, (unsigned char *)&val, 4);
	return val;
}

unsigned int readStartMultiplier()
{
	unsigned int val = 0;
	Read_DATAFLASH_ARRAY(DATA_START_ADDR+ START_MULTIPLIER_OFFSET, (unsigned char *)&val, 4);
	return val;
}
unsigned char readStopCounter(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR+ STOP_COUNTER_OFFSET);
}

unsigned char readStartCounter(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR + START_COUNTER_OFFSET);
}
unsigned char readWaterShortLevel(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR+ LEVEL_WATER_SHORT_OFFSET);
}

unsigned char readWaterShortDelay(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR + DELAY_WATER_SHORT_OFFSET);
}

void writeFlash(void)
{
	unsigned char mode = 0;
	if (start_multiplier == 40)
		mode = LEFT_FUNCTION_1_MODE;
	else if(start_multiplier == 400)
		mode = LEFT_FUNCTION_2_MODE;
	else if(start_multiplier == 400*6)
		mode = LEFT_FUNCTION_3_MODE;
	else if(start_multiplier == 400*6*10)
		mode = LEFT_FUNCTION_4_MODE;
	else if(start_multiplier == 400*6*10*6)
		mode = LEFT_FUNCTION_5_MODE;
	else if(start_multiplier == 400*6*10*6*10)
		mode = LEFT_FUNCTION_6_MODE;
	else {
		start_multiplier = 40;
		mode = LEFT_FUNCTION_1_MODE;
	}
	Write_DATAFLASH_BYTE(DATA_START_ADDR + LEFT_MODE_OFFSET, mode);
	Write_DATAFLASH_ARRAY(DATA_START_ADDR + START_MULTIPLIER_OFFSET, (unsigned char *)&start_multiplier, 4);
	if (stop_multiplier == 40)
		mode = RIGHT_FUNCTION_1_MODE;
	else if(stop_multiplier == 400)
		mode = RIGHT_FUNCTION_2_MODE;
	else if(stop_multiplier == 400*6)
		mode = RIGHT_FUNCTION_3_MODE;
	else if(stop_multiplier == 400*6*10)
		mode = RIGHT_FUNCTION_4_MODE;
	else if(stop_multiplier == 400*6*10*6)
		mode = RIGHT_FUNCTION_5_MODE;
	else if(stop_multiplier == 400*6*10*6*10)
		mode = RIGHT_FUNCTION_6_MODE;
	else {
		stop_multiplier = 40;
		mode = RIGHT_FUNCTION_1_MODE;
	}
	Write_DATAFLASH_BYTE(DATA_START_ADDR + RIGHT_MODE_OFFSET, mode);
	Write_DATAFLASH_ARRAY(DATA_START_ADDR + STOP_MULTIPLIER_OFFSET, (unsigned char *)&stop_multiplier, 4);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + START_COUNTER_OFFSET, start_counter%100);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + STOP_COUNTER_OFFSET, stop_counter%100);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + LEVEL_WATER_SHORT_OFFSET, level_water_short%2);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + DELAY_WATER_SHORT_OFFSET, delay_water_short%100);
	//flushFlash();
}

unsigned char readLeftMode(void)
{
	unsigned char mode = Read_APROM_BYTE(DATA_START_ADDR + LEFT_MODE_OFFSET);
	if ((mode < LEFT_FUNCTION_1_MODE || mode > LEFT_TIME_EFFECTIVE_MODE) && mode != LEARN_MODE)
		return LEFT_FUNCTION_1_MODE;
	else
		return mode;
}

unsigned char readRightMode(void)
{
	unsigned char mode = Read_APROM_BYTE(DATA_START_ADDR + RIGHT_MODE_OFFSET);
	if ((mode < RIGHT_FUNCTION_1_MODE || mode > RIGHT_TIME_EFFECTIVE_MODE) && mode != LEARN_MODE)
		return RIGHT_FUNCTION_1_MODE;
	else
		return mode;
}

