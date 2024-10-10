#include "business.h"
#include "Application/button.h"
#include "Application/nv.h"
#include "Application/output.h"
#include "Application/systick.h"
#include "tm1650.h"
#include "nv.h"
#include "output.h"
#include "systick.h"
#include "button.h"

extern unsigned int flag_collaborate;

extern unsigned int tButton;

unsigned int water_is_not_short(sprayerNvType *nv, unsigned char *spraying)
{
	unsigned int water_present = TRUE;
	unsigned int now = get_Timer1_Systemtick();
	nv->level_water_short = readWaterShortLevel();
	while (nv->level_water_short == external_water_short_blocked(*nv) && get_Timer1_Systemtick() - now < 30*60*400)
	{
		// water short configurations can be changed in this situation
    	nv->level_water_short = readWaterShortLevel();
		relay = 0;
		*spraying = SPRAY_WAITING;
		water_present = FALSE;
		beeper_2hz();
		if (tButton == RF_STOP_BUTTON
		|| TRUE == external_stop_valid_blocked()
		|| (flag_collaborate && collaborate_ex_button == 1)) // waiting can be forcibly stopped
		{

			beeper_once();
			beeper_2hz_stop();
			*spraying = SPRAY_IDLE;
			tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
			return FALSE;
		}
		else if(tButton == RF_START_BUTTON) // waiting can not be skipped
		{
			beeper_once();
			tButton = 0;
		}
	}
	beeper_2hz_stop();
	nv->level_water_short = readWaterShortLevel();
	if(water_present == FALSE && nv->level_water_short == external_water_short_blocked(*nv)) // water is still short 
	{
		relay = 0;
		*spraying = SPRAY_IDLE;
		return FALSE;
	}
	else if(water_present == FALSE && nv->level_water_short != external_water_short_blocked(*nv)) // water is not short now
	{
        unsigned int delay_water_short_backup = 0;            
        nv->delay_water_short = readWaterShortDelay();
        delay_water_short_backup = nv->delay_water_short;
		*spraying = SPRAY_DELAYING;
		nv->delay_water_short = readWaterShortDelay();
		while(0 < nv->delay_water_short)
		{
			unsigned int temp = 0;
			unsigned int prev = 0;
			temp = get_Timer1_Systemtick();
			while (get_Timer1_Systemtick() - temp < 400) {
				if(nv->level_water_short == external_water_short_blocked(*nv))
				{
					return TRUE;
				}
				else if (tButton == RF_STOP_BUTTON
				|| external_stop_valid_blocked()
				|| (flag_collaborate && collaborate_ex_button == 1)) // waiting can be forcibly stopped
				{
					beeper_once();
					tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					return FALSE;
				}
				else if (tButton == RF_START_BUTTON
				|| (external_start_valid_blocked() && !external_stop_valid_blocked())
				|| (external_collaborate_valid_blocked() && !external_stop_valid_blocked()))
				{
					beeper_once();
					tButton = (tButton == RF_START_BUTTON ? 0 : tButton);
					return TRUE;
				}
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 1);
			}
			nv->delay_water_short --;
		}
		nv->delay_water_short = delay_water_short_backup;
		*spraying = SPRAY_IDLE;
        return TRUE;
	}else if (water_present) // water is never short
	{
		return TRUE;
	}
	else{
		*spraying = SPRAY_IDLE;
		return FALSE;
	}
}
unsigned char start_spray(sprayerNvType *nv, unsigned char *spraying)
{
	unsigned int start_counter_backup =nv->start_counter%100;
	unsigned int cnt = nv->start_multiplier;
	unsigned int now = 0;
	if (nv->left_mode == LEFT_TIME_EFFECTIVE_MODE)
	{
		while(nv->start_counter>0)
		{
			// start spraying
			relay = 1;
			*spraying = SPRAY_STARTING;
			cnt = nv->start_multiplier;
			now = get_Timer1_Systemtick();
			while(get_Timer1_Systemtick() - now < cnt){
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				if (tButton == RF_STOP_BUTTON
					|| external_stop_valid_blocked()
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					beeper_once();
					relay = 0;
					nv->start_counter = start_counter_backup;
                    tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					return FALSE;
				}
				else if(tButton == RF_START_BUTTON)
				{
					beeper_once();
					tButton = (tButton == RF_START_BUTTON ? 0 : tButton);
				}
				else if(nv->level_water_short == external_water_short_blocked(*nv))
				{
					relay = 0; 
					nv->start_counter = start_counter_backup;
					return TRUE;
				}
			}
			nv->start_counter --;
		}
	}
	nv->start_counter = start_counter_backup;
	return TRUE;
}

unsigned char stop_spray(sprayerNvType *nv, unsigned char *spraying)
{
	unsigned int cnt = nv->stop_multiplier;
	unsigned int stop_counter_backup = nv->stop_counter%100;
	unsigned int now = 0;
	if (nv->right_mode == RIGHT_TIME_EFFECTIVE_MODE)
	{
		while(nv->stop_counter > 0)
		{
			relay = 0;
			*spraying = SPRAY_STOPPING;
			cnt = nv->stop_multiplier;
			now =  get_Timer1_Systemtick();
			while(get_Timer1_Systemtick() - now < cnt){
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				if (tButton == RF_STOP_BUTTON
					|| external_stop_valid_blocked()
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					beeper_once();
					nv->stop_counter = stop_counter_backup;
					tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					return FALSE;
				}
				else if(tButton == RF_START_BUTTON)
				{
					beeper_once();
					tButton = 0;
				}
				else if (nv->level_water_short == external_water_short_blocked(*nv))
				{
				    nv->stop_counter = stop_counter_backup;
					return TRUE;
				}
			}
			nv->stop_counter --;
		}
	}
	nv->stop_counter = stop_counter_backup;
	return TRUE;
}

static unsigned char is_learn_button_long_pressed(void)
{
	unsigned int now = get_Timer1_Systemtick();
	while (learn_button == 0)
	{
		Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 2);
		if (get_Timer1_Systemtick() - now > 800)
			return LEARN_LONG_BUTTON;
	}
	return 0;
}
void learn_code(sprayerNvType *nv, unsigned int* learning)
{
	unsigned int now = get_Timer1_Systemtick();
	unsigned int now1 = 0;
	if (nv->left_mode != LEARN_MODE || nv->right_mode != LEARN_MODE)
		return;
	while(get_Timer1_Systemtick() - now < 400*8 && tButton == 0)
	{
		if (is_learn_button_long_pressed())
		{
			beeper_once();
			clearAddress();
			break;
		}
		*learning = TRUE;
		Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
	}
	if(tButton == RF_START_BUTTON || tButton == RF_STOP_BUTTON)
	{
		*learning = FALSE;
		beeper_once();
		saveToFlash();
		now1 = get_Timer1_Systemtick();
		while(get_Timer1_Systemtick() -	now1 < 400)
			Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
		nv->left_mode = LEFT_TIME_EFFECTIVE_MODE;
		nv->right_mode = RIGHT_TIME_EFFECTIVE_MODE;
		tButton = 0;
		return;
	}
	*learning = FALSE;
	Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 800);
	nv->left_mode = LEFT_TIME_EFFECTIVE_MODE;
	nv->right_mode = RIGHT_TIME_EFFECTIVE_MODE;
	tButton = 0;
	return;
}

