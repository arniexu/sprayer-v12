#include "TM1650.h"
#include "I2C_GPIO.h"
#include "timer.h"
static uint8_t TM1650_CDigits[128] = {
//0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x00
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10
  0x00, 0x82, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00, // 0x20
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53, // 0x30
  0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x06, 0x1E, 0x00, 0x38, 0x00, 0x54, 0x3F, // 0x40
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x00, 0x0F, 0x00, 0x08, // 0x50 
  0x63, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x02, 0x1E, 0x00, 0x06, 0x00, 0x54, 0x5C, // 0x60
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x30, 0x0F, 0x00, 0x00  // 0x70
};
static unsigned char buttonCodes[] = {
	0x44, 0x45, 0x46, 0x47,
	0x4c, 0x4d, 0x4e, 0x4f,
	0x54, 0x55, 0x56, 0x57,
	0x5c, 0x5d, 0x5e, 0x5f,
	0x64, 0x65, 0x66, 0x67,
	0x6c, 0x6d, 0x6e, 0x6f,
	0x74, 0x75, 0x76, 0x77,
	0xa4};


static struct TM1650TypeDef tm1650;
static unsigned char displayBuffer[4] ={0};
static unsigned char blinkFlag[4] = {FALSE, FALSE, FALSE, FALSE};
static unsigned char dotBlinkFlag[4] = {FALSE, FALSE, FALSE, FALSE};
static unsigned char dotFlag[4] = {FALSE, FALSE, FALSE, FALSE};

/** Turns the display on
 */
void tm1650_displayOn(struct TM1650TypeDef *tm) {
	tm = NULL;
	I2C_Write_Byte(TM1650_DCTRL_BASE, 1);
}

/** Turns the display off
 */
void tm1650_displayOff(struct TM1650TypeDef *tm) {
	tm = NULL;
	I2C_Write_Byte(TM1650_DCTRL_BASE, 0);
}

/** Display string on the display
 * aString = character array to be displayed
 *
 * Internal buffer is updated as well
 * Only first N positions of the string are displayed if
 *  the string is longer than the number of digits
 */
void tm1650_displayString(struct TM1650TypeDef *tm, char *aString) {
	aString = NULL;
	tm = NULL;
	return;
}

/** Display character on the display
 * aPos = the position of the display character
 * aData = character to be displayed
 * aDot = display the dot if true, clear if false
 *
 * Display of the character at the specified position of the display.
 */
void tm1650_displayChar(struct TM1650TypeDef *tm, uint8_t aPos,  uint8_t c)
{
	tm = NULL;
	tm->iBuffer[aPos] = c;
	I2C_Write_Byte(TM1650_DISPLAY_BASE + aPos*2, TM1650_CDigits[c]);
	//HAL_I2C_Transmit(&hi2c1, TM1650_DISPLAY_BASE + aPos*2, TM1650_CDigits, 1 , HAL_MAX_DELAY);
}

/** Getting the buttons pressed
 *
 * returns: the code of the pressed buttons, for details refer to the documentation on the chip TM1650
 */
uint8_t tm1650_getButtons(struct TM1650TypeDef *tm) {
	tm = NULL;
	return I2C_Read_Byte(TM1650_DCTRL_BASE+1);
}

unsigned char isButtonCodeValid(UINT8 c)
{
	unsigned char i = 0;
	for (i = 0; i<29; i++)
		if (c == buttonCodes[i])
			return 1;
	return 0;
}

void display_when_learning()
{
	if(right_mode == LEARN_MODE && left_mode == LEARN_MODE)
	{
		displayBuffer[0] = '-';
		displayBuffer[1] = '-';
		displayBuffer[2] = '-';
		displayBuffer[3] = '-';
		blinkFlag[0] = blinkFlag[2] = blinkFlag[3] = SOLID_ON;
		blinkFlag[1] = SLOW_BLINK;
		dotFlag[0] = dotFlag[2] = dotFlag[1] = FALSE;
		dotFlag[3] = TRUE;
	}
	//os_set_ready(1);
	//os_switch_task();
}

void display_when_water_is_back()
{
	displayBuffer[0] = 'F';
	displayBuffer[1] = 'C';
	displayBuffer[2] = '0'+ delay_water_short/10;
	displayBuffer[3] = '0'+ delay_water_short%10;
	blinkFlag[0] = blinkFlag[1] = blinkFlag[2] = blinkFlag[3] = SOLID_ON;
	dotFlag[0] = dotFlag[1] = dotFlag[2] = dotFlag[3] = FALSE;
}


void display_when_water_is_short()
{
	// circular waiting
	displayBuffer[0] = '0';
	displayBuffer[1] = '0';
	displayBuffer[2] = '0';
	displayBuffer[3] = '0';
	blinkFlag[0] = blinkFlag[1] = blinkFlag[2] = blinkFlag[3] = WAIT_WATER;
	dotFlag[0] = dotFlag[2] = FALSE;
	dotFlag[1] = dotFlag[3] = TRUE;
}
void display_when_learn_complete()
{
	if (right_mode == LEARN_MODE && left_mode == LEARN_MODE) {
		displayBuffer[0] = '-';
		displayBuffer[1] = '-';
		displayBuffer[2] = '-';
		displayBuffer[3] = '-';
		blinkFlag[0] = blinkFlag[1] = blinkFlag[2] = blinkFlag[3] = FAST_BLINK;
		dotFlag[0] = dotFlag[1] = dotFlag[2] = FALSE;
		dotFlag[3] = FALSE;
	}
	//os_set_ready(1);
	//os_switch_task();
}

void refresh_left_display()
{
	switch(left_mode)
	{
		case LEFT_FUNCTION_1_MODE:
		case LEFT_FUNCTION_2_MODE:
		case LEFT_FUNCTION_3_MODE:
		case LEFT_FUNCTION_4_MODE:
		case LEFT_FUNCTION_5_MODE:
		case LEFT_FUNCTION_6_MODE:
			displayBuffer[0] = 'F';
			if (left_mode == LEFT_FUNCTION_1_MODE)
			{
				displayBuffer[1] = '1';
			}
			else if (left_mode == LEFT_FUNCTION_2_MODE)
			{
				displayBuffer[1] = '2';
			}
			else if (left_mode == LEFT_FUNCTION_3_MODE)
			{
				displayBuffer[1] = '3';
			}
			else if (left_mode == LEFT_FUNCTION_4_MODE)
			{
				displayBuffer[1] = '4';
			}
			else if (left_mode == LEFT_FUNCTION_5_MODE)
			{
				displayBuffer[1] = '5';
			}
			else if (left_mode == LEFT_FUNCTION_6_MODE)
			{
				displayBuffer[1] = '6';
			}
			blinkFlag[0] = FALSE;
			blinkFlag[1] = TRUE;
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
		case LEFT_TIME_EFFECTIVE_MODE:
			if (spraying == SPRAY_IDLE || spraying == SPRAY_STARTING)
			{
				blinkFlag[0] = blinkFlag[1] = FALSE;
				displayBuffer[1] = '0'+start_counter%10;
				displayBuffer[0] = '0'+(start_counter/10)%10;
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
			else
			{
				displayBuffer[1] = 0;
				displayBuffer[0] = 0;
				dotFlag[0] = dotFlag[1] = FALSE;
				blinkFlag[0] = blinkFlag[1] = FALSE;
			}
			break;
		case LEFT_FUNCTION_7_MODE:
			dotFlag[0] = dotFlag[1] = FALSE;
			displayBuffer[0] = 'F';
			displayBuffer[1] = '7';
			blinkFlag[0] = FALSE;
			blinkFlag[1] = TRUE;
			break;
		case LEFT_FUNCTION_8_MODE:
			dotFlag[0] = dotFlag[1] = FALSE;
			displayBuffer[0] = 'F';
			displayBuffer[1] = '8';
			blinkFlag[0] = FALSE;
			blinkFlag[1] = TRUE;
			break;
		default:
			break;
	}
}

void refresh_right_display()
{
	switch(right_mode)
	{
		case RIGHT_FUNCTION_1_MODE:
		case RIGHT_FUNCTION_2_MODE:
		case RIGHT_FUNCTION_3_MODE:
		case RIGHT_FUNCTION_4_MODE:
		case RIGHT_FUNCTION_5_MODE:
		case RIGHT_FUNCTION_6_MODE:
			displayBuffer[2] = 'F';
			if (right_mode == RIGHT_FUNCTION_1_MODE)
			{
				displayBuffer[3] = '1';
			}
			else if (right_mode == RIGHT_FUNCTION_2_MODE)
			{
				displayBuffer[3] = '2';
			}
			else if (right_mode == RIGHT_FUNCTION_3_MODE)
			{
				displayBuffer[3] = '3';
			}
			else if (right_mode == RIGHT_FUNCTION_4_MODE)
			{
				displayBuffer[3] = '4';
			}
			else if (right_mode == RIGHT_FUNCTION_5_MODE)
			{
				displayBuffer[3] = '5';
			}
			else if (right_mode == RIGHT_FUNCTION_6_MODE)
			{
				displayBuffer[3] = '6';
			}
			blinkFlag[2] = FALSE;
			blinkFlag[3] = TRUE;
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
		case RIGHT_TIME_EFFECTIVE_MODE:
			if (spraying == SPRAY_IDLE || spraying == SPRAY_STOPPING)
			{
				displayBuffer[3] = '0'+stop_counter%10;
				displayBuffer[2] = '0'+(stop_counter/10)%10;
				blinkFlag[2] = blinkFlag[3] = FALSE;
				if(stop_multiplier == 40
					|| stop_multiplier == 40*10*6
					|| stop_multiplier == 40*10*6*10*6)
				{
					dotFlag[2] = TRUE;
					dotFlag[3] = FALSE;
				}
				else
					dotFlag[2] = dotFlag[3] = FALSE;
			}
			else if(spraying == SPRAY_WAITING)
			{
				display_when_water_is_short();
			}
			else if(spraying == SPRAY_DELAYING)
			{
				display_when_water_is_back();
			}
			else
			{
				displayBuffer[3] = 0;
				displayBuffer[2] = 0;
				dotFlag[2] = dotFlag[3] = FALSE;
				blinkFlag[2] = blinkFlag[3] = FALSE;
			}
			break;
		case RIGHT_FUNCTION_7_MODE:
			dotFlag[2] = FALSE;
			dotFlag[3] = TRUE;
			blinkFlag[2] = FALSE;
			blinkFlag[3] = TRUE;
			displayBuffer[2] = 0;
			displayBuffer[3] = '0' + level_water_short%2;
			break;
		case RIGHT_FUNCTION_8_MODE:
			dotFlag[2] = FALSE;
			dotFlag[3] = FALSE;
			displayBuffer[2] = '0'+ (delay_water_short/10)%10;
			displayBuffer[3] = '0' + delay_water_short%10;
			blinkFlag[2] = blinkFlag[3] = TRUE;
			break;
		default:
			break;
	}
}

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
		refresh_left_display();
		refresh_right_display();
		for (i=0; i<4; i++)
		{
			if(blinkFlag[i])
			{
				unsigned short period = 0;
				if (blinkFlag[i] == FAST_BLINK)
				{
					period = 100;
					if(get_Timer1_Systemtick()%period < period/2)
					{
						tm1650_displayChar(&tm1650, i, 0);
					}
					else
					{
						if(dotFlag[i])
							tm1650_displayChar_withDot(&tm1650, i, displayBuffer[i]);
						else
							tm1650_displayChar(&tm1650, i, displayBuffer[i]);
					}
				}
				else if(blinkFlag[i] == SLOW_BLINK)
				{
					period = 400;
					if(get_Timer1_Systemtick()%period < period/2)
					{
						tm1650_displayChar(&tm1650, i, 0);
					}
					else
					{
						if(dotFlag[i])
							tm1650_displayChar_withDot(&tm1650, i, displayBuffer[i]);
						else
							tm1650_displayChar(&tm1650, i, displayBuffer[i]);
					}
				}
				else if(blinkFlag[i] == CIRCULAR_WAITING)
				{
					period = 1200;
					if(get_Timer1_Systemtick()%1200 < 200 && get_Timer1_Systemtick()%1200 > 0)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],1);
					else if(get_Timer1_Systemtick()%1200 < 400 && get_Timer1_Systemtick()%1200 > 200)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x02);
					else if(get_Timer1_Systemtick()%1200 < 600 && get_Timer1_Systemtick()%1200 > 400)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x03);
					else if(get_Timer1_Systemtick()%1200 < 800 && get_Timer1_Systemtick()%1200 > 600)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x04);
					else if(get_Timer1_Systemtick()%1200 < 1000 && get_Timer1_Systemtick()%1200 > 800)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x5);
					else if(get_Timer1_Systemtick()%1200 < 1200 && get_Timer1_Systemtick()%1200 > 1000)
						tm1650_displaySegment(&tm1650, i, displayBuffer[i],0x06);
				}
				else if(blinkFlag[i] == WAIT_WATER)
				{
					tm1650_displayChar_withDot_underMask(&tm1650, i, displayBuffer[i], 0x8c);
				}
			}
			else
			{
				if(dotFlag[i])
					tm1650_displayChar_withDot(&tm1650, i, displayBuffer[i]);
				else
					tm1650_displayChar(&tm1650, i, displayBuffer[i]);
			}
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
					continue;
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
		}
		//Timer1_Delay2Dot54ms_blocked(get_Timer1_Systemtick(), 10);
		os_switch_task();
	}
}
