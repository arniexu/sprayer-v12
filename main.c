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
#include "Application/nv.h"
#include "button.h"
#include "systick.h"
#include "business.h"
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

extern unsigned int tButton;
extern unsigned int button;

extern unsigned int spraying;

void business_logic() _task_ 2
{
	unsigned char previous = 0;
	sprayerNvType nv = {0};
	gpio_button_init_poll();
	input_signal_init();
	readFlash(&nv);
	left_button_logic(&nv);
	right_button_logic(&nv);
	while(1)
	{
		button = gpio_button_poll_blocked(previous);	
		if (isButtonCodeValid(button) && (spraying == SPRAY_IDLE || spraying == SPRAY_WAITING))
		{
			if (previous != button)  // button is not pressed consecutively
				beeper_once();
			left_button_logic(&nv);
			right_button_logic(&nv);
			previous = button;
			button = 0;
			left_button_logic(&nv);
			right_button_logic(&nv);
			writeFlash(nv);
			if (nv.right_mode == LEARN_MODE
				&& nv.left_mode == LEARN_MODE)
			{
				learn_code(nv);
			}
		}
		previous = button;
	}
}


/* rf button starts the cycle immediately 
   stop*/
// start up task to bring other tasks up
int startup_task (void) _task_ 0
{
	unsigned int start = 0, end = 0;
	unsigned int flag_collaborate = 0;
	sprayerNvType nv = {0};
	MODIFY_HIRC(HIRC_24);
	P10_QUASI_MODE;
	enable_Timer2_IC2();
	start_Timer1_Systemtick();
	input_signal_init();
	readFlash(&nv);
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
		if (nv.right_mode == RIGHT_TIME_EFFECTIVE_MODE
			&& nv.left_mode == LEFT_TIME_EFFECTIVE_MODE)
		{
			if (nv.level_water_short != external_water_short_blocked(nv))
			{	// 不缺水
				beeper_2hz_stop();
				spraying = SPRAY_IDLE;
				if (tButton == RF_START_BUTTON  // start is pressed from the remote controller
					|| (TRUE == external_start_valid_blocked()	// external start signal is valid and external stop signal is not valid
					&& FALSE == external_stop_valid_blocked()))
				{
					beeper_once();
					tButton = 0;
					while(water_is_not_short(nv) && start_spray(nv) && stop_spray(nv))
					{
						Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
						nv.start_counter = readStartCounter();
						nv.stop_counter = readStopCounter();
						nv.stop_multiplier = readStopMultiplier();
						nv.start_multiplier = readStartMultiplier();
					}
					spraying = SPRAY_IDLE;
				}
				else if(collaborate_ex_button == 0
					&& FALSE == external_stop_valid_blocked())
				{
					beeper_once();
					flag_collaborate = 1;
					while(collaborate_ex_button == 0 && water_is_not_short(nv) && start_spray(nv) && stop_spray(nv))
					{
						Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
						nv.start_counter = readStartCounter();
						nv.stop_counter = readStopCounter();
						nv.stop_multiplier = readStopMultiplier();
						nv.start_multiplier = readStartMultiplier();
					}
					flag_collaborate = 0;
					spraying = SPRAY_IDLE;
				}
				else if(tButton == RF_STOP_BUTTON)
				{
					beeper_once();
					tButton = 0;
				}
			}
			else 
			{
				// 缺水
				relay_ineffective();
				display_when_water_is_short();
				spraying = SPRAY_WAITING;
				beeper_2hz();
			}
		}	// 右侧或者左侧还在设置时间
		else if(tButton != 0)
		{
			beeper_once();
			tButton = 0;
		}
		//os_switch_task();
	}
}
