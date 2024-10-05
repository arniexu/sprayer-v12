#include "business.h"
#include "tm1650.h"
#include "nv.h"
#include "output.h"
#include "systick.h"
#include "button.h"

unsigned int stopReason = 0;
unsigned int flag_collaborate = 0;

extern unsigned int button;
extern unsigned int tButton;
unsigned int spraying;
unsigned int learning;

unsigned int water_is_not_short(sprayerNvType nv)
{
	unsigned int water_present = TRUE;
	unsigned int now = get_Timer1_Systemtick();
	nv.level_water_short = readWaterShortLevel();
	while (nv.level_water_short == external_water_short_blocked(nv) && get_Timer1_Systemtick() - now < 30*60*400)
	{
		// water short configurations can be changed in this situation
    nv.level_water_short = readWaterShortLevel();
		relay_ineffective();
		display_when_water_is_short();
		water_present = FALSE;
		beeper_2hz();
		if (tButton == RF_STOP_BUTTON
		|| TRUE == external_stop_valid_blocked()
		|| (flag_collaborate && collaborate_ex_button == 1)) // waiting can be forcibly stopped
		{

			beeper_once();
            beeper_2hz_stop();
            spraying = SPRAY_IDLE;
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
	if(water_present == FALSE  && nv.level_water_short == external_water_short_blocked(nv)) // water is still short 
	{
		relay_ineffective();
		spraying = SPRAY_IDLE;
		return FALSE;
	}
	else if(water_present == FALSE && nv.level_water_short != external_water_short_blocked(nv)) // water is not short now
	{
        unsigned int delay_water_short_backup = 0;            
        nv.delay_water_short = readWaterShortDelay();
        delay_water_short_backup = nv.delay_water_short;
		spraying = SPRAY_DELAYING;
		while(0 < nv.delay_water_short)
		{
			unsigned int temp = 0;
			unsigned int prev = 0;
			display_when_water_is_back(nv);
			temp = get_Timer1_Systemtick();
			while (get_Timer1_Systemtick() - temp < 400) {
				if(nv.level_water_short == external_water_short_blocked(nv))
				{
					return TRUE;
				}
				else if (tButton == RF_STOP_BUTTON
				|| external_stop_valid_blocked())
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
			nv.delay_water_short --;
		}
		nv.delay_water_short = delay_water_short_backup;
		Write_DATAFLASH_BYTE(DATA_START_ADDR+DELAY_WATER_SHORT_OFFSET, nv.delay_water_short%100);
		spraying = SPRAY_IDLE;
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
unsigned char start_spray(sprayerNvType nv)
{
	unsigned int start_counter_backup =nv.start_counter%100;
	unsigned int cnt = nv.start_multiplier;
	unsigned int now = 0;
	if (nv.left_mode == LEFT_TIME_EFFECTIVE_MODE)
	{
		while(nv.start_counter>0)
		{
			// start spraying
			relay_effective();
			spraying = SPRAY_STARTING;
			cnt = nv.start_multiplier;
			now = get_Timer1_Systemtick();
			while(get_Timer1_Systemtick() - now < cnt){
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				if (tButton == RF_STOP_BUTTON
					|| RF_STOP_BUTTON == external_button_poll_blocked()
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					beeper_once();
					relay_ineffective();
					nv.start_counter = start_counter_backup;
                    tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					Write_DATAFLASH_BYTE(DATA_START_ADDR+START_COUNTER_OFFSET, nv.start_counter%100);
					return FALSE;
				}
				else if(tButton == RF_START_BUTTON)
				{
					beeper_once();
					tButton = (tButton == RF_START_BUTTON ? 0 : tButton);
				}
				else if(nv.level_water_short == external_water_short_blocked(nv))
				{
					relay_ineffective(); 
					nv.start_counter = start_counter_backup;
					Write_DATAFLASH_BYTE(DATA_START_ADDR+START_COUNTER_OFFSET, nv.start_counter%100);
					return TRUE;
				}
			}
			nv.start_counter --;
		}
	}
	nv.start_counter = start_counter_backup;
	Write_DATAFLASH_BYTE(DATA_START_ADDR+START_COUNTER_OFFSET, nv.start_counter);
	return TRUE;
}

unsigned char stop_spray(sprayerNvType nv)
{
	unsigned int cnt = nv.stop_multiplier;
	unsigned int stop_counter_backup = nv.stop_counter%100;
	unsigned int now = 0;
	if (nv.right_mode == RIGHT_TIME_EFFECTIVE_MODE)
	{
		while(nv.stop_counter > 0)
		{
			relay_ineffective();
			spraying = SPRAY_STOPPING;
			cnt = nv.stop_multiplier;
			now =  get_Timer1_Systemtick();
			while(get_Timer1_Systemtick() - now < cnt){
				Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
				if (tButton == RF_STOP_BUTTON
					|| RF_STOP_BUTTON == external_button_poll_blocked()
					|| (flag_collaborate && collaborate_ex_button == 1))
				{
					beeper_once();
					nv.stop_counter = stop_counter_backup;
                    tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					Write_DATAFLASH_BYTE(DATA_START_ADDR+STOP_COUNTER_OFFSET, nv.stop_counter%100);
					return FALSE;
				}
				else if(tButton == RF_START_BUTTON)
				{
					beeper_once();
					tButton = 0;
				}
				else if (nv.level_water_short == external_water_short_blocked(nv))
				{
				    nv.stop_counter = stop_counter_backup;
					Write_DATAFLASH_BYTE(DATA_START_ADDR+STOP_COUNTER_OFFSET, nv.stop_counter%100);
					return TRUE;
				}
			}
			nv.stop_counter --;
		}
	}
	nv.stop_counter = stop_counter_backup;
	Write_DATAFLASH_BYTE(DATA_START_ADDR+STOP_COUNTER_OFFSET, nv.stop_counter%100);
	return TRUE;
}

void learn_code(sprayerNvType nv)
{
	unsigned int now = get_Timer1_Systemtick();
	unsigned int now1 = 0;
	while(get_Timer1_Systemtick() - now < 400*8 && tButton == 0)
	{
		learning = TRUE;
		display_when_learning(nv);
		Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
	}
	if(tButton == RF_START_BUTTON || tButton == RF_STOP_BUTTON)
	{
		learning = FALSE;
		beeper_once();
		//writeAddressToFlash();
		display_when_learn_complete(nv);
		now1 = get_Timer1_Systemtick();
		while(get_Timer1_Systemtick() -	now1 < 400)
			Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 1);
		nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
		nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
		tButton = 0;
		return;
	}
	learning = FALSE;
	display_when_learn_complete(nv);
	Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 800);
	nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
	nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
	tButton = 0;
	return;
}

