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

#if 0
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
#endif

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

/*
功能描述：蜂鸣器叫一声
实现方式：阻塞时和非阻塞式，当前为非阻塞式实现；阻塞式实现方式可以通过#if 0打开
注意点： 连续的多次蜂鸣并不会连在一起形成一声长长的嘀，而是嘀嘀嘀一声声分开的
注意点2：不建议使用阻塞式实现
*/
void beeper_once(void) 
{
	#if 1
	if(!beeper_start)
		beeper_start = get_Timer1_Systemtick();
	else
		return;
	beeperFlag |= BEEP_ONCE;
	#else
	beeper = 1;
	Timer1_Delay2Dot54ms_Unblocked(get_Timer1_Systemtick(), 200);
	beeper = 0;
	#endif
}

/*
功能描述：2HZ蜂鸣
*/
void beeper_2hz(void)
{
	beeperFlag &= ~0x7f;
	beeperFlag += BEEP_2HZ;
}

/*
功能描述：停止2hz蜂鸣
*/
void beeper_2hz_stop()
{
	beeperFlag &= ~0x7f;
}

/*
功能描述：蜂鸣器蜂鸣执行线程
功能支持：1HZ 2HZ 4HZ 8HZ 蜂鸣和单独的蜂鸣一声
注意点：此函数需要至少1ms调用一次，可以放在定时器的中断处理函数或者一个独立线程中
注意点2：此时的系统滴答声是2.54ms
*/
int beeper_job()
{
	if(beeperFlag)
	{
		unsigned int period = 0;
		if(beeperFlag & BEEP_ONCE)
		{
			if(get_Timer1_Systemtick() - beeper_start < 40)
				beeper = 1;
			else
			{
				beeper_start = 0;
				beeperFlag &= ~BEEP_ONCE;
				beeper = 0;
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
			beeper = 1;
		}
		else
		{
			beeper = 0;
		}
	}
	else 
	{
		beeper = 0;	
	}
	return 0;
}
