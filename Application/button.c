#include "button.h"
#include "timer.h"
#include "systick.h"
#include <RTX51TNY.h>

extern unsigned int button;

#if 0
void PinInterrupt_ISR (void) interrupt 7
{
_push_(SFRS);
  
     if (PIF&=SET_BIT2)
     {
       P02 ^= 1;
     }
     if (PIF&=SET_BIT2)
     {
       P03 ^= 1;
     }

_pop_(SFRS);
}
/******************************************************************************
The main C function.  Program execution starts
here after stack initialization.
******************************************************************************/
void gpio_button_init_interrupt(void) 
{
		RIGHT_UP_BUTTON_PIN_INIT;
		RIGHT_DOWN_BUTTON_PIN_INIT;
		RIGHT_SET_BUTTON_PIN_INIT;
		LEFT_UP_BUTTON_PIN_INIT;
		LEFT_DOWN_BUTTON_PIN_INIT;
		LEFT_SET_BUTTON_PIN_INIT;
		LEARN_BUTTON_PIN_INIT;
/*----------------------------------------------------*/
/*  P1.3 set as highlevel trig pin interrupt function */
/*  otherwise, MCU into idle mode.                    */
/*----------------------------------------------------*/

    ENABLE_INT_PORT0;
    ENABLE_BIT0_FALLINGEDGE_TRIG;	
    ENABLE_BIT5_FALLINGEDGE_TRIG;
    ENABLE_BIT6_FALLINGEDGE_TRIG;
    ENABLE_BIT1_FALLINGEDGE_TRIG;
    ENABLE_INT_PORT1;
    ENABLE_BIT1_FALLINGEDGE_TRIG;
    ENABLE_BIT5_FALLINGEDGE_TRIG;
    ENABLE_BIT4_FALLINGEDGE_TRIG;
    set_EIE_EPI;                            // Enable pin interrupt
    ENABLE_GLOBAL_INTERRUPT;                // global enable bit
}

#endif

void gpio_button_init_poll (void) 
{
		RIGHT_UP_BUTTON_PIN_INIT;
		RIGHT_DOWN_BUTTON_PIN_INIT;
		RIGHT_SET_BUTTON_PIN_INIT;
		LEFT_UP_BUTTON_PIN_INIT;
		LEFT_DOWN_BUTTON_PIN_INIT;
		LEFT_SET_BUTTON_PIN_INIT;
		LEARN_BUTTON_PIN_INIT;
}

#if 0 
static unsigned char keys_pressed[NUM_BUTTONS] = {0};

unsigned char gpio_button_poll_unblocked(void)
{
	if (right_up_button == 0)
	{	 
		if (keys_pressed[RIGHT_UP_BUTTON_ID]++ > 4)
		{
			keys_pressed[RIGHT_UP_BUTTON_ID] = 0;
			return RIGHT_UP_BUTTON;
		}
	}
	else
		keys_pressed[RIGHT_UP_BUTTON_ID] = 0;
	if (right_down_button == 0)
	{	 
		if (keys_pressed[RIGHT_DOWN_BUTTON_ID]++ > 4)
		{
			keys_pressed[RIGHT_DOWN_BUTTON_ID] = 0;
			return RIGHT_DOWN_BUTTON;
		}
	}
	else
		keys_pressed[RIGHT_DOWN_BUTTON_ID] = 0;
	if (right_set_button == 0)
	{	 
		if (keys_pressed[RIGHT_SET_BUTTON_ID]++ > 4)
		{
			keys_pressed[RIGHT_SET_BUTTON_ID] = 0;
			return RIGHT_SET_BUTTON;
		}
	}
	else
		keys_pressed[RIGHT_SET_BUTTON_ID] = 0;
	if (left_up_button == 0)
	{	 
		if (keys_pressed[LEFT_UP_BUTTON_ID]++ > 4)
		{
			keys_pressed[LEFT_UP_BUTTON_ID] = 0;
			return LEFT_UP_BUTTON;
		}
	}
	else
		keys_pressed[LEFT_UP_BUTTON_ID] = 0;
	if (left_down_button == 0)
	{	 
		if (keys_pressed[LEFT_DOWN_BUTTON_ID]++ > 4)
		{
			keys_pressed[LEFT_DOWN_BUTTON_ID] = 0;
			return LEFT_DOWN_BUTTON;
		}
	}
	else
		keys_pressed[LEFT_DOWN_BUTTON_ID] = 0;
	if (left_set_button == 0)
	{	 
		if (keys_pressed[LEFT_SET_BUTTON_ID]++ > 4)
		{
			keys_pressed[LEFT_SET_BUTTON_ID] = 0;
			return LEFT_SET_BUTTON;
		}
	}
	else
		keys_pressed[LEFT_SET_BUTTON_ID] = 0;
	return 0;
}
#endif
unsigned char gpio_button_poll_blocked(unsigned char prev)
{
		unsigned int now = 0;
		unsigned int duration = 0;
		if (right_up_button == 0)
		{
			if(prev == RIGHT_UP_BUTTON)
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 2);
			else
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
			if (right_up_button == 0)
				return RIGHT_UP_BUTTON;
		}
		else if(right_down_button == 0)
		{
			if(prev == RIGHT_DOWN_BUTTON)
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 2);
			else
			{
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
			}
				if (right_down_button == 0)
					return RIGHT_DOWN_BUTTON;
		}
		else if(right_set_button == 0)
		{	
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
				if (right_set_button == 0)
					return RIGHT_SET_BUTTON;
		}
		else if(left_up_button == 0)
		{
			if(prev == LEFT_UP_BUTTON)
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 2);
			else
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
			if (left_up_button == 0)
				return LEFT_UP_BUTTON;
		}
		else if(left_down_button == 0)
		{
			if(prev == LEFT_DOWN_BUTTON)
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 2);
			else
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
				if (left_down_button == 0)
					return LEFT_DOWN_BUTTON;
		}
		else if(left_set_button == 0)
		{
			now = get_Timer1_Systemtick();
			while(left_set_button == 0) {
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 1);
				duration = get_Timer1_Systemtick() - now;
			}
			if (duration > 200)
				return LEFT_SET_LONG_BUTTON;
			else if(duration > 10)
				return LEFT_SET_BUTTON;
		}
		else if(learn_button == 0)
		{
				Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
				if (learn_button == 0)
					return LEARN_BUTTON;
		}
		return 0;
}

#if 0
unsigned char gpio_button_id(unsigned char cod)
{
	switch(cod)
	{
		case RIGHT_UP_BUTTON: return RIGHT_UP_BUTTON_ID;
		case RIGHT_DOWN_BUTTON: return RIGHT_DOWN_BUTTON_ID;
		case RIGHT_SET_BUTTON: return RIGHT_SET_BUTTON_ID;
		case LEFT_UP_BUTTON: return LEFT_UP_BUTTON_ID;
		case LEFT_DOWN_BUTTON: return LEFT_DOWN_BUTTON_ID;
		case LEFT_SET_BUTTON: return LEFT_SET_BUTTON_ID;
		case LEARN_BUTTON: return LEARN_BUTTON_ID;
		default : return 0xff;
	}
}

unsigned char wait_button_blocked(void)
{
	while(SIG_EVENT != os_wait(K_SIG, 500, 0));
	return 1;
}
#endif
