#include "output.h"
#include "systick.h"

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
