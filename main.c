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
#include "Application/output.h"
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

extern unsigned int tButton;

unsigned char spraying = SPRAY_IDLE;
unsigned int learning = FALSE;
sprayerNvType nv = {0};

void business_logic() _task_ 2
{
	unsigned char previous = 0;
	unsigned char button = 0;
	gpio_button_init_poll();
	input_signal_init();
	readFlash(&nv);
	nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
	nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
	spraying = SPRAY_IDLE;
	left_button_logic(button);
	right_button_logic(button);
	while(1)
	{
		button = gpio_button_poll_blocked(previous);	
		if (isButtonCodeValid(button) && (spraying == SPRAY_IDLE || spraying == SPRAY_WAITING))
		{
			if (previous != button)  // button is not pressed consecutively
				beeper_once();
			left_button_logic(button);
			right_button_logic(button);
			previous = button;
			button = 0;
			left_button_logic(button);
			right_button_logic(button);
			writeFlash(nv); 
			if (nv.right_mode == LEARN_MODE
				&& nv.left_mode == LEARN_MODE)
			{
				learn_code(&nv, &learning);
			}
		}
		previous = button;
		//os_switch_task();
	}
}

/* rf button starts the cycle immediately 
   stop*/
// start up task to bring other tasks up
int startup_task (void) _task_ 0
{
	unsigned int start = 0, end = 0;
	unsigned int flag_collaborate = 0;
	MODIFY_HIRC(HIRC_24);
	P10_QUASI_MODE;
	enable_Timer2_IC2();
	start_Timer1_Systemtick();
	input_signal_init();
	beeper = 0;
	os_create_task(1);
	os_create_task(2);
	relay = 0;
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
					while(water_is_not_short(&nv, &spraying) && start_spray(&nv, &spraying) && stop_spray(&nv, &spraying))
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
					while(collaborate_ex_button == 0 && water_is_not_short(&nv, &spraying) && start_spray(&nv, &spraying) && stop_spray(&nv, &spraying))
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
					spraying = SPRAY_IDLE;
					beeper_once();
					tButton = 0;
				}
			}
			else 
			{
				// 缺水
				relay = 0;
				spraying = SPRAY_WAITING;
				beeper_2hz();
			}
		}	// 右侧或者左侧还在设置时间
		else if(tButton != 0)
		{
			spraying = SPRAY_IDLE;
			beeper_once();
			tButton = 0;
		}
		os_switch_task();
	}
}

extern sprayerDisplayType led;
extern struct TM1650TypeDef tm1650;
// refresh led display
void refresh_led (void) _task_ 1
{
	unsigned int i = 0;
	// start a timer as system tick handler, time out every 1ms 
	// count the number of	
	IIC_Init();
	tm1650_displayOn(&tm1650);
	while(1)
	{
		refresh_left_display(nv);
		refresh_right_display(nv);
		for (i=0; i<4; i++)
		{
			if(led.blinkFlag[i])
			{
				unsigned short period = 0;
				if (led.blinkFlag[i] == FAST_BLINK)
				{
					period = 100;
					if(get_Timer1_Systemtick()%period < period/2)
					{
						tm1650_displayChar(&tm1650, i, 0);
					}
					else
					{
						if(led.dotFlag[i])
							tm1650_displayChar_withDot_underMask(&tm1650, i, led.displayBuffer[i], 0xff);
						else
							tm1650_displayChar(&tm1650, i, led.displayBuffer[i]);
					}
				}
				else if(led.blinkFlag[i] == SLOW_BLINK)
				{
					period = 400 ;
					if(get_Timer1_Systemtick()%period < period/2)
					{
						tm1650_displayChar(&tm1650, i, 0);
					}
					else
					{
						if(led.dotFlag[i])
							tm1650_displayChar_withDot_underMask(&tm1650, i, led.displayBuffer[i], 0xff);
						else
							tm1650_displayChar(&tm1650, i, led.displayBuffer[i]);
					}
				}
				#if 0
				else if(led.blinkFlag[i] == CIRCULAR_WAITING)
				{
					period = 1200;
					if(get_Timer1_Systemtick()%1200 < 200 && get_Timer1_Systemtick()%1200 > 0)
						tm1650_displaySegment(&tm1650, i, led.displayBuffer[i],1);
					else if(get_Timer1_Systemtick()%1200 < 400 && get_Timer1_Systemtick()%1200 > 200)
						tm1650_displaySegment(&tm1650, i, led.displayBuffer[i],0x02);
					else if(get_Timer1_Systemtick()%1200 < 600 && get_Timer1_Systemtick()%1200 > 400)
						tm1650_displaySegment(&tm1650, i, led.displayBuffer[i],0x03);
					else if(get_Timer1_Systemtick()%1200 < 800 && get_Timer1_Systemtick()%1200 > 600)
						tm1650_displaySegment(&tm1650, i, led.displayBuffer[i],0x04);
					else if(get_Timer1_Systemtick()%1200 < 1000 && get_Timer1_Systemtick()%1200 > 800)
						tm1650_displaySegment(&tm1650, i, led.displayBuffer[i],0x5);
					else if(get_Timer1_Systemtick()%1200 < 1200 && get_Timer1_Systemtick()%1200 > 1000)
						tm1650_displaySegment(&tm1650, i, led.displayBuffer[i],0x06);
				}
				#endif
				else if(led.blinkFlag[i] == WAIT_WATER)
				{
					tm1650_displayChar_withDot_underMask(&tm1650, i, led.displayBuffer[i], 0x8c);
				}
			}
			else
			{
				if(led.dotFlag[i])
					tm1650_displayChar_withDot_underMask(&tm1650, i, led.displayBuffer[i], 0xff);
				else
					tm1650_displayChar(&tm1650, i, led.displayBuffer[i]);
			}
		}
		if (beeper_job() < 0) // beeper once is not complete yet
		{
			continue;
		}
		//Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
		os_switch_task();
	}
}
