#include "business.h"

unsigned int stopReason = 0;
unsigned int flag_collaborate = 0;
//@TODO: save below 4 variables into flash
unsigned int start_multiplier = 0;
unsigned int start_counter = 0;
unsigned int stop_multiplier = 0;
unsigned int stop_counter = 0;
unsigned int level_water_short = 1;
unsigned int delay_water_short = 0;

unsigned char right_mode = RIGHT_TIME_EFFECTIVE_MODE;
unsigned char left_mode = LEFT_TIME_EFFECTIVE_MODE;

unsigned int spraying = SPRAY_IDLE;

unsigned int learning = TRUE;

extern unsigned int button;

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
            else // spraying is stopping
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
                // 缺水等待状态时，看代码是可以支持设置 
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
		// 缺水状态下不能被任意信号停止，无线按钮仅用于测试
		if (tButton == RF_STOP_BUTTON
			|| TRUE == external_stop_valid_blocked()
			|| (flag_collaborate && collaborate_ex_button == 1)) // waiting can be forcibly stopped
		{
			beeper_once();
			tButton = (RF_STOP_BUTTON == tButton ? 0 : tButton);
			return FALSE;
		}
		else if(tButton == RF_START_BUTTON)
		{
			beeper_once();
			tButton = (RF_START_BUTTON == tButton ? 0 : tButton);
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
			unsigned int prev = 0;
			display_when_water_is_back();
			temp = get_Timer1_Systemtick();
			while (get_Timer1_Systemtick() - temp < 400) {
				if(level_water_short == external_water_short_blocked())
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
					tButton = (tButton == RF_STOP_BUTTON ? 0 : tButton);
					Write_DATAFLASH_BYTE(DATA_START_ADDR+START_COUNTER_OFFSET, start_counter%100);
					return FALSE;
				}
				else if(tButton == RF_START_BUTTON)
				{
					beeper_once();
					tButton = (tButton == RF_START_BUTTON ? 0 : tButton);
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
