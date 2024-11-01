#include "button.h"
#include "nv.h"
#include "business.h"
#include "timer.h"
#include "systick.h"
#include <RTX51TNY.h>
extern sprayerNvType nv;
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

/*
����������������ʽ�ķ�ʽ��ȡ��������
����������֧��7���������ֱ���������µ��ں�������ü����Ҳ����µ��ں��Ҳ����ü�������һ��ѧ�밴��
�������ܣ�������ü�֧�ֳ����Ͷ̰���������֧�ֶ̰�������
�̰����壺����ʱ����75ms��2s֮�伴Ϊ�̰�
�������壺����ʱ����2s���ϼ�Ϊ���������������¼�ǰ�����ж̰��¼�
�������壺����ʱ����75ms���Ͼ�Ϊ������75ms�Ժ�ÿ5ms����һ�ΰ����¼�
*/
static unsigned int left_set_pressed = FALSE;
static unsigned int left_set_button_start = 0;
static unsigned int right_set_button_pressed = FALSE;
static unsigned int right_set_button_start = 0;
static unsigned int right_up_button_start = 0;
static unsigned int right_up_button_pressed = 0;
static unsigned int right_down_button_start = 0;
static unsigned int right_down_button_pressed = 0;
static unsigned int left_up_button_start = 0;
static unsigned int left_up_button_pressed = 0;
static unsigned int left_down_button_start = 0;
static unsigned int left_down_button_pressed = 0;
unsigned char gpio_button_poll_blocked(unsigned char prev)
{
        if(left_set_button == 0)
        {
            if(left_set_button_start > 0)
            {
                if (get_Timer1_Systemtick() - left_set_button_start > 800
                    && left_set_pressed == 1)
                {
                    left_set_pressed = 2;
                    return LEFT_SET_LONG_BUTTON;
                }
                else if (get_Timer1_Systemtick() - left_set_button_start > 20
                    && !left_set_pressed)
                {
                    left_set_pressed = 1;
                    return LEFT_SET_BUTTON;
                }
                else if(left_set_pressed == 5)
                {
                    ; // nothing 	
                }
            }
            else {
                left_set_pressed = 0;
                left_set_button_start = get_Timer1_Systemtick();
            }
        }
        else
        {
            left_set_button_start = 0;
            left_set_pressed = 0;
        }
        if (right_up_button == 0)
        {
            if(right_up_button_start > 0)
            {
                if(get_Timer1_Systemtick() - right_up_button_start > 400)
                {
                    if (right_up_button_pressed == 1)
                    {
                        right_up_button_pressed = 2;
                        return RIGHT_UP_BUTTON;
                    }
                    else if(right_up_button_pressed == 2)
                    {
												right_up_button_pressed = 3;
                        if ((get_Timer1_Systemtick() - right_up_button_start) % 5 == 4)
                            return RIGHT_UP_BUTTON;
                    }
                }
                else if(get_Timer1_Systemtick() - right_up_button_start > 20
                    && !right_up_button_pressed)
                {
                    right_up_button_pressed = 1;
                    return RIGHT_UP_BUTTON;
                }
            }
            else
            {
                right_up_button_pressed = 0;
                right_up_button_start = get_Timer1_Systemtick();
            }
        }
        else
        {
            right_up_button_pressed = 0;
            right_up_button_start = 0;
        }

        if(right_down_button == 0)
        {
            if(right_down_button_start > 0)
            {
                if(get_Timer1_Systemtick() - right_down_button_start > 400)
                {
                    if (right_down_button_pressed == 1)
                    {
                        right_down_button_pressed = 2;
                        return RIGHT_DOWN_BUTTON;
                    }
                    else if(right_down_button_pressed == 2)
                    {
												right_down_button_pressed = 3;
                        if ((get_Timer1_Systemtick() - right_down_button_start) % 5 == 4)
                            return RIGHT_DOWN_BUTTON;
                    }
                }
                else if(get_Timer1_Systemtick() - right_down_button_start > 20
                    && !right_down_button_pressed)
                {
                    right_down_button_pressed = 1;
                    return RIGHT_DOWN_BUTTON;
                }
            }
            else
            {
                right_down_button_pressed = 0;
                right_down_button_start = get_Timer1_Systemtick();
            }
        }
        else
        {
            right_down_button_pressed = 0;
            right_down_button_start = 0;
        }
        if(right_set_button == 0)
        {
            if(right_set_button_start > 0)
            {
                if(get_Timer1_Systemtick() - right_set_button_start > 20
                    && !right_set_button_pressed)
                {
                    right_set_button_pressed = 1;
                    return RIGHT_SET_BUTTON;
                }
            }
            else
            {
                right_set_button_pressed = 0;
                right_set_button_start = get_Timer1_Systemtick();
            }
        }
        else
        {
            right_set_button_pressed = 0;
            right_set_button_start = 0;
        }
        if(left_up_button == 0)
        {
            if(left_up_button_start > 0)
            {
                if(get_Timer1_Systemtick() - left_up_button_start > 400)
                {
                    if (left_up_button_pressed == 1)
                    {
                        left_up_button_pressed = 2;
                        return LEFT_UP_BUTTON;
                    }
                    else if(left_up_button_pressed == 2)
                    {
												left_up_button_pressed = 3;
                        if ((get_Timer1_Systemtick() - left_up_button_start) % 5 == 4)
                            return LEFT_UP_BUTTON;
                    }
                }
                else if(get_Timer1_Systemtick() - left_up_button_start > 20
                    && !left_up_button_pressed)
                {
                    left_up_button_pressed = 1;
                    return LEFT_UP_BUTTON;
                }
            }
            else
            {
                left_up_button_pressed = 0;
                left_up_button_start = get_Timer1_Systemtick();
            }
        }
        else
        {
            left_up_button_pressed = 0;
            left_up_button_start = 0;
        }
        if(left_down_button == 0)
        {
            if(left_down_button_start > 0)
            {
                if(get_Timer1_Systemtick() - left_down_button_start > 400)
                {
                    if (left_down_button_pressed == 1)
                    {
                        left_down_button_pressed = 2;
                        return LEFT_DOWN_BUTTON;
                    }
                    else if(left_down_button_pressed == 2)
                    {
												left_down_button_pressed = 3;
                        if ((get_Timer1_Systemtick() - left_down_button_start) % 5 == 4)
                            return LEFT_DOWN_BUTTON;
                    }
                }
                else if(get_Timer1_Systemtick() - left_down_button_start > 20
                    && !left_down_button_pressed)
                {
                    left_down_button_pressed = 1;
                    return LEFT_DOWN_BUTTON;
                }
            }
            else
            {
                left_down_button_pressed = 0;
                left_down_button_start = get_Timer1_Systemtick();
            }
        }
        else
        {
            left_down_button_pressed = 0;
            left_down_button_start = 0;
        }
        if(learn_button == 0)
        {
            Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 50);
            if (learn_button == 0)
                return LEARN_BUTTON;
        }
		return 0;
}

unsigned char no_beep_for_button()
{
	if(right_up_button_pressed == 3
		|| right_down_button_pressed == 3
		|| left_up_button_pressed == 3
		|| left_down_button_pressed == 3)
		return TRUE;
	return FALSE;
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

// left business logic
/*
����������������ð����̰����������F1ģʽ��������ϵ�����F2���µ�����F6
��ʱ�����������6����ʱ������ֱ���0.1s��1s��0.1min��1min��0.1h��1h��������ӦF1��F6
�������ܣ�������ü����̰�����Fxģʽ���ٴζ̰��˳�Fxģʽ������ϵ����µ�������F1-F6���л�ģʽ 
*/
void left_button_logic(unsigned int button) 
{
    switch(nv.left_mode)
    {
        // ��ʱ���0.1s
        case LEFT_FUNCTION_1_MODE:
            // ���ö�ʱ���
            nv.start_multiplier = 40; // 0.1s
            // ������ð����̰�
            if (button == LEFT_SET_BUTTON)
                nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
            // ����ϵ������̰�
            else if(button == LEFT_UP_BUTTON)
                nv.left_mode = LEFT_FUNCTION_2_MODE;
            // ����µ������̰�
            else if(button == LEFT_DOWN_BUTTON)
                nv.left_mode = LEFT_FUNCTION_6_MODE;
            // ���ѧ�밴���̰�
            else if (button == LEARN_BUTTON)
            {
                nv.left_mode = LEARN_MODE;
            }
            // ������ð�������
            else if (button == LEFT_SET_LONG_BUTTON)
            {
                nv.left_mode = LEFT_FUNCTION_7_MODE;
                nv.right_mode = RIGHT_FUNCTION_7_MODE;
            }
            break;
        // ��ʱ���1s
        case LEFT_FUNCTION_2_MODE:
            nv.start_multiplier = 400; // 1s
            // wait_button_blocked
            if (button == LEFT_SET_BUTTON)
                nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
            else if(button == LEFT_UP_BUTTON)
                nv.left_mode = LEFT_FUNCTION_3_MODE;
            else if(button == LEFT_DOWN_BUTTON)
                nv.left_mode = LEFT_FUNCTION_1_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.left_mode = LEARN_MODE;
            }
            else if (button == LEFT_SET_LONG_BUTTON)
            {

                nv.left_mode = LEFT_FUNCTION_7_MODE;
                nv.right_mode = RIGHT_FUNCTION_7_MODE;
            }
            break;
        case LEFT_FUNCTION_3_MODE:
            nv.start_multiplier = 2400; //0.1min
            // wait_button_blocked
            if (button == LEFT_SET_BUTTON)
                nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
            else if(button == LEFT_UP_BUTTON)
                nv.left_mode = LEFT_FUNCTION_4_MODE;
            else if(button == LEFT_DOWN_BUTTON)
                nv.left_mode = LEFT_FUNCTION_2_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.left_mode = LEARN_MODE;
            }
            else if (button == LEFT_SET_LONG_BUTTON)
            {
                nv.left_mode = LEFT_FUNCTION_7_MODE;
                nv.right_mode = RIGHT_FUNCTION_7_MODE;
            }
            break;
        case LEFT_FUNCTION_4_MODE:
            nv.start_multiplier = 24000; //1min
            // wait_button_blocked
            if (button == LEFT_SET_BUTTON)
                nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
            else if(button == LEFT_UP_BUTTON)
                nv.left_mode = LEFT_FUNCTION_5_MODE;
            else if(button == LEFT_DOWN_BUTTON)
                nv.left_mode = LEFT_FUNCTION_3_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.left_mode = LEARN_MODE;
            }
            else if (button == LEFT_SET_LONG_BUTTON)
            {
                nv.left_mode = LEFT_FUNCTION_7_MODE;
                nv.right_mode = RIGHT_FUNCTION_7_MODE;
            }
            break;
        case LEFT_FUNCTION_5_MODE:
            nv.start_multiplier = 144000; //6 min
            // wait_button_blocked
            if (button == LEFT_SET_BUTTON)
                nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
            else if(button == LEFT_UP_BUTTON)
                nv.left_mode = LEFT_FUNCTION_6_MODE;
            else if(button == LEFT_DOWN_BUTTON)
                nv.left_mode = LEFT_FUNCTION_4_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.left_mode = LEARN_MODE;
            }
            else if (button == LEFT_SET_LONG_BUTTON)
            {
                nv.left_mode = LEFT_FUNCTION_7_MODE;
                nv.right_mode = RIGHT_FUNCTION_7_MODE;
            }
            break;
        case LEFT_FUNCTION_6_MODE:
            nv.start_multiplier = 60*60*400; //60 min
            // wait_button_blocked
            if (button == LEFT_SET_BUTTON)
                nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
            else if(button == LEFT_UP_BUTTON)
                nv.left_mode = LEFT_FUNCTION_1_MODE;
            else if(button == LEFT_DOWN_BUTTON)
                nv.left_mode = LEFT_FUNCTION_5_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.left_mode = LEARN_MODE;
            }
            else if (button == LEFT_SET_LONG_BUTTON)
            {
                nv.left_mode = LEFT_FUNCTION_7_MODE;
                nv.right_mode = RIGHT_FUNCTION_7_MODE;
            }
            break;
        case LEFT_TIME_EFFECTIVE_MODE:
            // wait_button_blocked
            if (button == LEFT_SET_BUTTON)
                nv.left_mode = readLeftMode();
            else if(button == LEFT_UP_BUTTON)
            {
                if (nv.start_counter == 99)
                    nv.start_counter = 1;
                else
                    nv.start_counter ++;

            }
            else if(button == LEFT_DOWN_BUTTON)
            {
                if (nv.start_counter == 1)
                    nv.start_counter = 99;
                else if (nv.start_counter > 1)
                    nv.start_counter --;
            }
            else if (button == LEARN_BUTTON)
            {
                nv.left_mode = LEARN_MODE;
            }
            else if (button == LEFT_SET_LONG_BUTTON)
            {
                nv.left_mode = LEFT_FUNCTION_7_MODE;
                nv.right_mode = RIGHT_FUNCTION_7_MODE;
            }
            break;
        case LEARN_MODE:
            break;
        case LEFT_FUNCTION_7_MODE:
            if(button == LEFT_SET_BUTTON){
                nv.right_mode = RIGHT_FUNCTION_8_MODE;
                nv.left_mode = LEFT_FUNCTION_8_MODE;
            }
            break;
        case LEFT_FUNCTION_8_MODE:
            if (button == LEFT_SET_BUTTON)
            {
                nv.left_mode = LEFT_TIME_EFFECTIVE_MODE;
                nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
            }
            break;
        default:
            break;
    }
	return;
}

// right business logic
void right_button_logic(unsigned int button) 
{
    switch(nv.right_mode)
    {
        case RIGHT_FUNCTION_1_MODE:
            nv.stop_multiplier = 40; // 0.1s
            // wait_button_blocked
            if (button == RIGHT_SET_BUTTON)
                nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
            else if(button == RIGHT_UP_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_2_MODE;
            else if(button == RIGHT_DOWN_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_6_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.right_mode = LEARN_MODE;
            }
            break;
        case RIGHT_FUNCTION_2_MODE:
            nv.stop_multiplier = 400; // 1s
            // wait_button_blocked
            if (button == RIGHT_SET_BUTTON)
                nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
            else if(button == RIGHT_UP_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_3_MODE;
            else if(button == RIGHT_DOWN_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_1_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.right_mode = LEARN_MODE;
            }
            break;
        case RIGHT_FUNCTION_3_MODE:
            nv.stop_multiplier = 2400; //0.1min
            // wait_button_blocked
            if (button == RIGHT_SET_BUTTON)
                nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
            else if(button == RIGHT_UP_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_4_MODE;
            else if(button == RIGHT_DOWN_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_2_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.right_mode = LEARN_MODE;
            }
            break;
        case RIGHT_FUNCTION_4_MODE:
            nv.stop_multiplier = 60*400; //1min
            // wait_button_blocked
            if (button == RIGHT_SET_BUTTON)
                nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
            else if(button == RIGHT_UP_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_5_MODE;
            else if(button == RIGHT_DOWN_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_3_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.right_mode = LEARN_MODE;
            }
            break;
        case RIGHT_FUNCTION_5_MODE:
            nv.stop_multiplier = 6*60*400; //6 min
            // wait_button_blocked
            if (button == RIGHT_SET_BUTTON)
                nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
            else if(button == RIGHT_UP_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_6_MODE;
            else if(button == RIGHT_DOWN_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_4_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.right_mode = LEARN_MODE;
            }
            break;
        case RIGHT_FUNCTION_6_MODE:
            nv.stop_multiplier = 60*60*400; //60 min
            // wait_button_blocked
            if (button == RIGHT_SET_BUTTON)
                nv.right_mode = RIGHT_TIME_EFFECTIVE_MODE;
            else if(button == RIGHT_UP_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_1_MODE;
            else if(button == RIGHT_DOWN_BUTTON)
                nv.right_mode = RIGHT_FUNCTION_5_MODE;
            else if (button == LEARN_BUTTON)
            {
                nv.right_mode = LEARN_MODE;
            }
            break;

        case RIGHT_TIME_EFFECTIVE_MODE:
            if (button == RIGHT_SET_BUTTON)
                nv.right_mode = readRightMode();
            else if(button == RIGHT_UP_BUTTON)
            {
                if (nv.stop_counter == 99)
                    nv.stop_counter = 0;
                else
                    nv.stop_counter ++;

            }
            else if(button == RIGHT_DOWN_BUTTON)
            {
                if (nv.stop_counter == 0)
                    nv.stop_counter = 99;
                else if(nv.stop_counter > 0)
                    nv.stop_counter --;
            }
            else if (button == LEARN_BUTTON)
            {
                nv.right_mode = LEARN_MODE;
            }
            break;
        case LEARN_MODE: 
            break;
        case RIGHT_FUNCTION_7_MODE:
            if(button == RIGHT_UP_BUTTON || button == RIGHT_DOWN_BUTTON)
                nv.level_water_short = (nv.level_water_short == 0 ? 1 : 0);
            break;
        case RIGHT_FUNCTION_8_MODE:
            if(button == RIGHT_UP_BUTTON)
                nv.delay_water_short ++;
            else if(button == RIGHT_DOWN_BUTTON)
                nv.delay_water_short = (nv.delay_water_short > 0 ? (nv.delay_water_short - 1) : 0);

            break;
        default:
            break;
    }
	return;
}

