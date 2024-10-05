#include "output.h"
#include "systick.h"
#include "nv.h"

static unsigned char beeperFlag = BEEP_OFF;
static unsigned int beeper_start = 0;

void input_signal_init()
{
	P12_QUASI_MODE;
	P13_QUASI_MODE;
	P17_QUASI_MODE;
	P30_QUASI_MODE;
	P03_PUSHPULL_MODE;
	P07_PUSHPULL_MODE;
}

void beeper_signal_effective()
{
	beeper = 1;
}

void beeper_signal_ineffective()
{
	beeper = 0;
}

void relay_effective()
{
	relay = 1;
}

void relay_ineffective()
{
	relay = 0;
}

unsigned char external_button_poll_blocked(void)
{
		if (start_ex_button == 0)
		{
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
				if (start_ex_button == 0)
					return RF_START_BUTTON;
		}
		else if(stop_ex_button == 0)
		{
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
				if (stop_ex_button == 0)
					return RF_STOP_BUTTON;
		}
		else if(collaborate_ex_button == 0)
		{
			Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
			if(collaborate_ex_button == 0)
				return RF_COLLABORATE_BUTTON;
		}
		return 0;
}

unsigned char external_start_valid_blocked()
{
	unsigned int i = 0;
	if (0 == start_ex_button)
	{
		for (i = 0; i<100; i++);
		if (0 == start_ex_button)
			return TRUE;
	}
	return FALSE;
}

unsigned char external_stop_valid_blocked()
{
	unsigned int i = 0;
	if (0 == stop_ex_button)
	{
		for(i = 0; i<100; i++);
		if (0 == stop_ex_button)
			return TRUE;
	}
	return FALSE;
}

unsigned char external_collaborate_valid_blocked()
{
	unsigned int i = 0;
	if (0 == collaborate_ex_button)
	{
		for(i = 0; i<100; i++);
		if (0 == collaborate_ex_button) {
			return TRUE;
		}
	}
	return FALSE;
}

unsigned char external_water_short_blocked(sprayerNvType nv)
{
	unsigned int i = 0;
	if (water_short_ex_button == nv.level_water_short)
	{
		for (i = 0; i<100; i++);
		if (water_short_ex_button == nv.level_water_short)
			return nv.level_water_short;
	}
	return !nv.level_water_short;
}


void beeper_once(void) 
{
	#if 1
	if(!beeper_start)
		beeper_start = get_Timer1_Systemtick();
	else
		return;
	beeperFlag |= BEEP_ONCE;
	#else
	beeper_signal_effective();
	Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 200);
	beeper_signal_ineffective();
	#endif
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

int beeper_job()
{
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
			return -1;
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
	return 0;
}
