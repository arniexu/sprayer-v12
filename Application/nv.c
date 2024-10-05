#include <MS51_16K.H>
#include "nv.h"
#include "iap.h"
#include "common.h"
#include "MS51_16K.h"

unsigned int readStopMultiplier()
{
	unsigned int val = 0;
	Read_DATAFLASH_ARRAY(DATA_START_ADDR+ STOP_MULTIPLIER_OFFSET, (unsigned char *)&val, 4);
	return val;
}

unsigned int readStartMultiplier()
{
	unsigned int val = 0;
	Read_DATAFLASH_ARRAY(DATA_START_ADDR+ START_MULTIPLIER_OFFSET, (unsigned char *)&val, 4);
	return val;
}
unsigned char readStopCounter(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR+ STOP_COUNTER_OFFSET);
}

unsigned char readStartCounter(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR + START_COUNTER_OFFSET);
}
unsigned char readWaterShortLevel(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR+ LEVEL_WATER_SHORT_OFFSET);
}

unsigned char readWaterShortDelay(void)
{
	return Read_APROM_BYTE(DATA_START_ADDR + DELAY_WATER_SHORT_OFFSET);
}

void writeFlash(void)
{
	unsigned char mode = 0;
	if (start_multiplier == 40)
		mode = LEFT_FUNCTION_1_MODE;
	else if(start_multiplier == 400)
		mode = LEFT_FUNCTION_2_MODE;
	else if(start_multiplier == 400*6)
		mode = LEFT_FUNCTION_3_MODE;
	else if(start_multiplier == 400*6*10)
		mode = LEFT_FUNCTION_4_MODE;
	else if(start_multiplier == 400*6*10*6)
		mode = LEFT_FUNCTION_5_MODE;
	else if(start_multiplier == 400*6*10*6*10)
		mode = LEFT_FUNCTION_6_MODE;
	else {
		start_multiplier = 40;
		mode = LEFT_FUNCTION_1_MODE;
	}
	Write_DATAFLASH_BYTE(DATA_START_ADDR + LEFT_MODE_OFFSET, mode);
	Write_DATAFLASH_ARRAY(DATA_START_ADDR + START_MULTIPLIER_OFFSET, (unsigned char *)&start_multiplier, 4);
	if (stop_multiplier == 40)
		mode = RIGHT_FUNCTION_1_MODE;
	else if(stop_multiplier == 400)
		mode = RIGHT_FUNCTION_2_MODE;
	else if(stop_multiplier == 400*6)
		mode = RIGHT_FUNCTION_3_MODE;
	else if(stop_multiplier == 400*6*10)
		mode = RIGHT_FUNCTION_4_MODE;
	else if(stop_multiplier == 400*6*10*6)
		mode = RIGHT_FUNCTION_5_MODE;
	else if(stop_multiplier == 400*6*10*6*10)
		mode = RIGHT_FUNCTION_6_MODE;
	else {
		stop_multiplier = 40;
		mode = RIGHT_FUNCTION_1_MODE;
	}
	Write_DATAFLASH_BYTE(DATA_START_ADDR + RIGHT_MODE_OFFSET, mode);
	Write_DATAFLASH_ARRAY(DATA_START_ADDR + STOP_MULTIPLIER_OFFSET, (unsigned char *)&stop_multiplier, 4);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + START_COUNTER_OFFSET, start_counter%100);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + STOP_COUNTER_OFFSET, stop_counter%100);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + LEVEL_WATER_SHORT_OFFSET, level_water_short%2);
	Write_DATAFLASH_BYTE(DATA_START_ADDR + DELAY_WATER_SHORT_OFFSET, delay_water_short%100);
	//flushFlash();
}

unsigned char readLeftMode(void)
{
	unsigned char mode = Read_APROM_BYTE(DATA_START_ADDR + LEFT_MODE_OFFSET);
	if ((mode < LEFT_FUNCTION_1_MODE || mode > LEFT_TIME_EFFECTIVE_MODE) && mode != LEARN_MODE)
		return LEFT_FUNCTION_1_MODE;
	else
		return mode;
}

unsigned char readRightMode(void)
{
	unsigned char mode = Read_APROM_BYTE(DATA_START_ADDR + RIGHT_MODE_OFFSET);
	if ((mode < RIGHT_FUNCTION_1_MODE || mode > RIGHT_TIME_EFFECTIVE_MODE) && mode != LEARN_MODE)
		return RIGHT_FUNCTION_1_MODE;
	else
		return mode;
}

